"""Файловые операции: туда-обратно на разных размерах, неверный ключ, повреждённые файлы, двоичный вывод."""
import os
import random
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

from core import aes, ecdsa, kuznyechik, magma, rsa, sha, streebog
from core.errors import CryptoError
from core.formatting import to_binary_string
from core.logger import set_log_sink

SIZES = (0, 1, 7, 8, 15, 16, 17, 1000, 65536, 70001, 1048576 + 7)


class FileOperations(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        set_log_sink(lambda s: None)
        cls.tmp = tempfile.TemporaryDirectory()
        cls.d = Path(cls.tmp.name)
        (cls.d / "out").mkdir()

    @classmethod
    def tearDownClass(cls):
        cls.tmp.cleanup()

    def make(self, size):
        f = self.d / f"f{size}.bin"
        f.write_bytes(random.Random(size).randbytes(size))
        return f

    def test_aes_roundtrip(self):
        for bits in (128, 192, 256):
            k = self.d / f"aes{bits}.key"
            aes.aes_generate_key(bits, k)
            for size in SIZES:
                f = self.make(size)
                enc = aes.aes_encrypt(f, self.d / "out", k, bits)
                self.assertEqual(enc.stat().st_size, ((size // 16) + 1) * 16 + 16)
                self.assertEqual(aes.aes_decrypt(enc, self.d / "out", k, bits).read_bytes(), f.read_bytes(), (bits, size))

    def test_aes_wrong_key_and_bad_file(self):
        k1, k2 = self.d / "a1.key", self.d / "a2.key"
        aes.aes_generate_key(256, k1)
        aes.aes_generate_key(256, k2)
        f = self.make(1000)
        rejected = 0
        for _ in range(30):
            aes.aes_generate_key(256, k1)
            enc = aes.aes_encrypt(f, self.d / "out", k1, 256)
            aes.aes_generate_key(256, k2)
            try:
                aes.aes_decrypt(enc, self.d / "out", k2, 256)
            except CryptoError:
                rejected += 1
        self.assertGreaterEqual(rejected, 27)          # ~1/256 случаев CBC без MAC даёт «мусор» вместо ошибки
        bad = self.d / "bad.aes"
        bad.write_bytes(b"12345678901")
        with self.assertRaises(CryptoError):
            aes.aes_decrypt(bad, self.d / "out", k1, 256)
        short_key = self.d / "short.key"
        short_key.write_bytes(b"123")
        with self.assertRaises(CryptoError):
            aes.aes_encrypt(f, self.d / "out", short_key, 256)

    def test_rsa_roundtrip(self):
        rsa.rsa_generate_keys(2048, self.d)
        for size in (0, 1, 1000, 300000):
            f = self.make(size)
            enc = rsa.rsa_encrypt(f, self.d / "out", self.d / "public_2048.key", 2048)
            self.assertEqual(rsa.rsa_decrypt(enc, self.d / "out", self.d / "private_2048.key", 2048).read_bytes(), f.read_bytes())
        other = self.d / "other"
        other.mkdir()
        rsa.rsa_generate_keys(2048, other)
        enc = rsa.rsa_encrypt(self.make(1000), self.d / "out", self.d / "public_2048.key", 2048)
        with self.assertRaises(CryptoError):           # чужой закрытый ключ
            rsa.rsa_decrypt(enc, self.d / "out", other / "private_2048.key", 2048)

    def test_ecdsa(self):
        f = self.make(300000)
        for curve in ecdsa.CURVES:
            priv, pub, sig = self.d / f"{curve}.key", self.d / f"{curve}_pub.key", self.d / f"{curve}.sig"
            self.assertEqual(ecdsa.ecdsa_public_key_path_for(priv), pub)
            ecdsa.ecdsa_generate_keys(curve, priv, pub)
            ecdsa.ecdsa_sign(f, priv, sig)
            self.assertTrue(ecdsa.ecdsa_verify(f, pub, sig), curve)
            self.assertFalse(ecdsa.ecdsa_verify(self.make(1000), pub, sig), curve)       # другой файл
            with self.assertRaises(CryptoError):
                ecdsa.ecdsa_sign(f, pub, sig)                                            # открытым ключом не подписать

    def test_gost_ciphers_roundtrip(self):
        for name, mod, gen, enc_fn, dec_fn in (
                ("magma", magma, magma.magma_generate_key, magma.magma_encrypt, magma.magma_decrypt),
                ("kuznyechik", kuznyechik, kuznyechik.kuznyechik_generate_key, kuznyechik.kuznyechik_encrypt, kuznyechik.kuznyechik_decrypt)):
            k = self.d / f"{name}.key"
            gen(k)
            bs = 8 if name == "magma" else 16
            for size in (0, 1, 7, 8, 15, 16, 17, 1000, 65537):
                f = self.make(size)
                info = enc_fn(f, self.d / "out", k)
                self.assertEqual(info.output_size, ((size // bs) + 1) * bs + bs, (name, size))
                self.assertEqual(len(info.iv), bs)
                back = dec_fn(info.output_file, self.d / "out", k)
                self.assertEqual(back.output_file.read_bytes(), f.read_bytes(), (name, size))

    def test_gost_wrong_key_cleans_up(self):
        k1, k2 = self.d / "mk1.key", self.d / "mk2.key"
        f = self.make(1000)
        rejected = 0
        for _ in range(30):
            magma.magma_generate_key(k1)
            info = magma.magma_encrypt(f, self.d / "out", k1)
            magma.magma_generate_key(k2)
            try:
                magma.magma_decrypt(info.output_file, self.d / "out", k2)
            except CryptoError:
                rejected += 1
                self.assertFalse((self.d / "out" / "f1000.bin.decrypted").exists())   # недописанный результат удалён
        self.assertGreaterEqual(rejected, 27)

    def test_streebog_file_equals_memory_and_binary_output(self):
        f = self.make(70001)
        for bits in (256, 512):
            self.assertEqual(streebog.compute_file_streebog(f, bits).digest, streebog.streebog_digest(f.read_bytes(), bits))
        text = streebog.hash_file_streebog(f, 512)
        rows = [l for l in text.split("\n") if l and l[0] in "01"]
        self.assertEqual(len(rows), 8)
        self.assertTrue(all(set(r) <= set("01 ") and len(r.split()) == 8 for r in rows))

    def test_sha256_output_is_binary(self):
        f = self.d / "abc.txt"
        f.write_bytes(b"abc")
        text = sha.hash_file_sha256(f)
        bits = "".join(text.split("(256 бит):\n")[1].split())
        self.assertEqual(int(bits, 2).to_bytes(32, "big").hex(),
                         "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad")   # SHA-256("abc")

    def test_to_binary_string(self):
        self.assertEqual(to_binary_string(bytes([0x9d, 0x15, 0x1e, 0xef, 0x00]), 4), "10011101 00010101 00011110 11101111\n00000000")


if __name__ == "__main__":
    unittest.main(verbosity=2)
