"""Проверка ГОСТ-алгоритмов по тестовым векторам RFC 8891 (Магма), RFC 7801 (Кузнечик), RFC 6986 (Стрибог)."""
import os
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

from core import kuznyechik as kz
from core import magma as mg
from core.gost_pi import PI
from core.streebog import StreebogHasher, streebog_digest, streebog_lps

h = bytes.fromhex


class MagmaRfc8891(unittest.TestCase):
    def test_t(self):                                   # A.1
        for a, b in ((0xfdb97531, 0x2a196f34), (0x2a196f34, 0xebd9f03a), (0xebd9f03a, 0xb039bb3d), (0xb039bb3d, 0x68695433)):
            self.assertEqual(mg.magma_t_spec(a), b)
            self.assertEqual(mg.magma_t(a), b)          # табличный вариант == определению

    def test_g(self):                                   # A.2
        for k, a, r in ((0x87654321, 0xfedcba98, 0xfdcbc20c), (0xfdcbc20c, 0x87654321, 0x7e791a4b),
                        (0x7e791a4b, 0xfdcbc20c, 0xc76549ec), (0xc76549ec, 0x7e791a4b, 0x9791c849)):
            self.assertEqual(mg.magma_g(k, a), r)

    def test_key_schedule_and_block(self):              # A.3 - A.5
        key = h("ffeeddccbbaa99887766554433221100f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff")
        rk = mg.magma_expand_key(key)
        k8 = [0xffeeddcc, 0xbbaa9988, 0x77665544, 0x33221100, 0xf0f1f2f3, 0xf4f5f6f7, 0xf8f9fafb, 0xfcfdfeff]
        self.assertEqual(rk, k8 * 3 + k8[::-1])
        pt, ct = h("fedcba9876543210"), h("4ee901e5c2d8ca3d")
        self.assertEqual(mg.magma_encrypt_block(pt, rk), ct)
        self.assertEqual(mg.magma_decrypt_block(ct, rk), pt)


class KuznyechikRfc7801(unittest.TestCase):
    def test_pi_inverse_table(self):                    # п. 4.1: проверка перенесённой таблицы
        self.assertEqual(sorted(PI), list(range(256)))
        self.assertTrue(all(kz.PI_INV[PI[i]] == i for i in range(256)))
        self.assertEqual(kz.PI_INV[:8], bytes([165, 45, 50, 143, 14, 48, 56, 192]))   # из RFC: Pi^-1 начинается так
        self.assertEqual(kz.PI_INV[-3:], bytes([76, 215, 116]))

    def test_s(self):                                   # 5.1
        a = h("ffeeddccbbaa99881122334455667700")
        for e in ("b66cd8887d38e8d77765aeea0c9a7efc", "559d8dd7bd06cbfe7e7b262523280d39",
                  "0c3322fed531e4630d80ef5c5a81c50b", "23ae65633f842d29c5df529c13f5acda"):
            a = a.translate(PI)
            self.assertEqual(a, h(e))

    def test_r_and_l(self):                             # 5.2, 5.3
        a = list(h("00000000000000000000000000000100"))
        for e in ("94000000000000000000000000000001", "a5940000000000000000000000000000",
                  "64a59400000000000000000000000000", "0d64a594000000000000000000000000"):
            a = kz.transform_r(a)
            self.assertEqual(bytes(a), h(e))
        pairs = (("64a59400000000000000000000000000", "d456584dd0e3e84cc3166e4b7fa2890d"),
                 ("d456584dd0e3e84cc3166e4b7fa2890d", "79d26221b87b584cd42fbc4ffea5de9a"),
                 ("79d26221b87b584cd42fbc4ffea5de9a", "0e93691a0cfc60408b7b68f66b513c13"),
                 ("0e93691a0cfc60408b7b68f66b513c13", "e6a8094fee0aa204fd97bcb0b44b8580"))
        for i, o in pairs:
            self.assertEqual(bytes(kz.transform_l(list(h(i)))), h(o))
            self.assertEqual(bytes(kz.transform_l_inv(list(h(o)))), h(i))

    def test_constants(self):                           # 5.4
        c = kz.tables().c
        exp = ("6ea276726c487ab85d27bd10dd849401", "dc87ece4d890f4b3ba4eb92079cbeb02", "b2259a96b4d88e0be7690430a44f7f03",
               "7bcd1b0b73e32ba5b79cb140f2551504", "156f6d791fab511deabb0c502fd18105", "a74af7efab73df160dd208608b9efe06",
               "c9e8819dc73ba5ae50f5b570561a6a07", "f6593616e6055689adfba18027aa2a08")
        for i, e in enumerate(exp):
            self.assertEqual(c[i].to_bytes(16, "big"), h(e))

    def test_key_schedule_and_block(self):              # 5.4 - 5.6
        key = h("8899aabbccddeeff0011223344556677fedcba98765432100123456789abcdef")
        rk = kz.kuz_expand_key(key)
        exp = ("8899aabbccddeeff0011223344556677", "fedcba98765432100123456789abcdef", "db31485315694343228d6aef8cc78c44",
               "3d4553d8e9cfec6815ebadc40a9ffd04", "57646468c44a5e28d3e59246f429f1ac", "bd079435165c6432b532e82834da581b",
               "51e640757e8745de705727265a0098b1", "5a7925017b9fdd3ed72a91a22286f984", "bb44e25378c73123a5f32f73cdb6e517",
               "72e9dd7416bcf45b755dbaa88e4a4043")
        for i, e in enumerate(exp):
            self.assertEqual(rk[i].to_bytes(16, "big"), h(e), f"K_{i + 1}")
        pt, ct = h("1122334455667700ffeeddccbbaa9988"), h("7f679d90bebc24305a468d42b9d4edcd")
        self.assertEqual(kz.kuz_encrypt_block(pt, rk), ct)
        self.assertEqual(kz.kuz_decrypt_block(ct, rk), pt)


class StreebogRfc6986(unittest.TestCase):
    # В RFC сообщения и хеши записаны числами «старший байт первым»; в памяти байты идут наоборот.
    M1 = h("323130393837363534333231303938373635343332313039383736353433323130393837363534333231303938373635343332313039383736353433323130")[::-1]
    M2 = h("fbe2e5f0eee3c820fbeafaebef20fffbf0e1e0f0f520e0ed20e8ece0ebe5f0f2f120fff0eeec20f120faf2fee5e2202ce8f6f3ede220e8e6eee1e8f0f2d1202ce8f0f2e5e220e5d1")[::-1]

    def test_m1_is_ascii_digits(self):
        self.assertEqual(self.M1, b"012345678901234567890123456789012345678901234567890123456789012")

    def test_k_lps_zero(self):                          # K = LPS(0^512)
        self.assertEqual(streebog_lps(0).to_bytes(64, "little")[::-1], h("b383fc2eced4a574" * 8))

    def test_hashes(self):
        cases = (
            (self.M1, 512, "486f64c1917879417fef082b3381a4e211c324f074654c38823a7b76f830ad00fa1fbae42b1285c0352f227524bc9ab16254288dd6863dccd5b9f54a1ad0541b"),
            (self.M1, 256, "00557be5e584fd52a449b16b0251d05d27f94ab76cbaa6da890b59d8ef1e159d"),
            (self.M2, 512, "28fbc9bada033b1460642bdcddb90c3fb3e56c497ccd0f62b8a2ad4935e85f037613966de4ee00531ae60f3b5a47f8dae06915d5f2f194996fcabf2622e6881e"),
            (self.M2, 256, "508f7e553c06501d749a66fc28c6cac0b005746d97537fa85d9e40904efed29d"),
        )
        for msg, bits, exp in cases:
            self.assertEqual(streebog_digest(msg, bits)[::-1], h(exp), f"{len(msg)} bytes, {bits} bit")

    def test_streaming_equals_one_shot(self):
        data = bytes((i * 31 + 7) & 0xFF for i in range(1000))
        for chunk in (1, 7, 64, 100):
            hs = StreebogHasher(512)
            for i in range(0, len(data), chunk):
                hs.update(data[i:i + chunk])
            self.assertEqual(hs.digest(), streebog_digest(data, 512), f"chunk {chunk}")

    def test_empty_message(self):                       # эталонное значение из независимых источников
        self.assertEqual(streebog_digest(b"", 256).hex(), "3f539a213e97c802cc229d474c6aa32a825a360b2a933a949fd925208d9ce1bb")


if __name__ == "__main__":
    unittest.main(verbosity=2)
