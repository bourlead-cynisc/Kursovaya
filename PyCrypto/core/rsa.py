"""Гибридное шифрование RSA-OAEP + AES-256-CBC на pycryptodome.
Формат файла: [uint32 LE длина][RSA-OAEP(AES-ключ)][IV 16 байт][AES-CBC шифртекст].
Ключи: private_<N>.key (PKCS#8 DER) и public_<N>.key (SubjectPublicKeyInfo DER)."""
import struct
from pathlib import Path

from Crypto.Cipher import AES, PKCS1_OAEP
from Crypto.PublicKey import RSA
from Crypto.Random import get_random_bytes
from Crypto.Util.Padding import pad, unpad

from .errors import CryptoError
from .files import ensure_dir, open_input, open_output, remove_quietly
from .formatting import format_file_size, format_ms
from .logger import log
from .timer import Timer

_CHUNK = 1 << 20


def _load_key(path, what: str):
    try:
        return RSA.import_key(Path(path).read_bytes())
    except OSError as e:
        raise CryptoError(f"Не удалось открыть {what}") from e
    except (ValueError, IndexError, TypeError) as e:
        raise CryptoError(f"Файл не является корректным RSA-ключом ({what})") from e


def rsa_generate_keys(key_size: int, key_dir) -> None:
    d = ensure_dir(key_dir, "Папка ключей")
    timer = Timer()
    key = RSA.generate(key_size)
    (d / f"private_{key_size}.key").write_bytes(key.export_key(format="DER", pkcs=8))
    (d / f"public_{key_size}.key").write_bytes(key.publickey().export_key(format="DER"))
    log(f"RSA {key_size}-bit ключи сгенерированы за {format_ms(timer.elapsed_ms())}")
    log(f"  Модуль: {len(str(key.n))} цифр")


def rsa_encrypt(input_file, output_dir, pub_key_file, key_size: int) -> Path:
    timer = Timer()
    out_dir = ensure_dir(output_dir)
    public_key = _load_key(pub_key_file, "открытый ключ")

    aes_key = get_random_bytes(32)
    iv = get_random_bytes(AES.block_size)
    encrypted_key = PKCS1_OAEP.new(public_key).encrypt(aes_key)   # OAEP + SHA-1, как RSAES_OAEP_SHA в Crypto++
    cipher = AES.new(aes_key, AES.MODE_CBC, iv)

    input_file = Path(input_file)
    out_path = out_dir / (input_file.name + ".rsa")
    with open_input(input_file) as fin, open_output(out_path) as fout:
        try:
            fout.write(struct.pack("<I", len(encrypted_key)))
            fout.write(encrypted_key)
            fout.write(iv)
            while True:
                chunk = fin.read(_CHUNK)
                if len(chunk) < _CHUNK:
                    fout.write(cipher.encrypt(pad(chunk, AES.block_size)))
                    break
                fout.write(cipher.encrypt(chunk))
        except BaseException:
            fout.close()
            remove_quietly(out_path)
            raise

    log(f"Шифрование RSA {key_size}-bit завершено за {format_ms(timer.elapsed_ms())}")
    log(f"  Размер файла: {format_file_size(input_file.stat().st_size)}")
    return out_path


def rsa_decrypt(input_file, output_dir, priv_key_file, key_size: int) -> Path:
    timer = Timer()
    out_dir = ensure_dir(output_dir)
    private_key = _load_key(priv_key_file, "закрытый ключ")
    if not private_key.has_private():
        raise CryptoError("Выбран открытый ключ, а нужен закрытый")

    bad_format = CryptoError("Файл не похож на зашифрованный RSA (неверный формат)")
    input_file = Path(input_file)
    total = input_file.stat().st_size if input_file.is_file() else 0
    bs = AES.block_size

    with open_input(input_file) as fin:
        head = fin.read(4)
        if len(head) != 4:
            raise bad_format
        key_len = struct.unpack("<I", head)[0]
        if key_len == 0 or key_len > 4096:
            raise bad_format
        encrypted_key = fin.read(key_len)
        iv = fin.read(bs)
        if len(encrypted_key) != key_len or len(iv) != bs:
            raise bad_format
        header = 4 + key_len + bs
        if total < header + bs or (total - header) % bs != 0:
            raise bad_format

        try:
            aes_key = PKCS1_OAEP.new(private_key).decrypt(encrypted_key)
        except ValueError:
            raise CryptoError("Не удалось расшифровать ключ: выбран не тот закрытый ключ") from None
        if len(aes_key) != 32:
            raise bad_format

        cipher = AES.new(aes_key, AES.MODE_CBC, iv)
        base = input_file.name
        if base.endswith(".rsa"):
            base = base[:-4]
        out_path = out_dir / (base + ".decrypted")
        with open_output(out_path) as fout:
            try:
                remaining = total - header
                while remaining > 0:
                    chunk = fin.read(min(_CHUNK, remaining))
                    remaining -= len(chunk)
                    plain = cipher.decrypt(chunk)
                    if remaining == 0:
                        plain = unpad(plain, bs)
                    fout.write(plain)
            except ValueError:
                fout.close()
                remove_quietly(out_path)
                raise CryptoError("Не удалось расшифровать: файл повреждён") from None
            except BaseException:
                fout.close()
                remove_quietly(out_path)
                raise

    log(f"Расшифровка RSA {key_size}-bit завершена за {format_ms(timer.elapsed_ms())}")
    log(f"  Размер файла: {format_file_size(out_path.stat().st_size)}")
    return out_path
