"""AES-CBC (PKCS#7) на pycryptodome. Формат файла: [IV 16 байт][шифртекст]."""
from pathlib import Path

from Crypto.Cipher import AES
from Crypto.Random import get_random_bytes
from Crypto.Util.Padding import pad, unpad

from .errors import CryptoError
from .files import (ensure_dir, open_input, open_output, read_raw_key_file,
                    remove_quietly, write_raw_key_file)
from .formatting import format_file_size, format_ms
from .logger import log
from .timer import Timer

_CHUNK = 1 << 20  # кратно размеру блока AES


def aes_generate_key(key_size_bits: int, key_file) -> bytes:
    if key_size_bits not in (128, 192, 256):
        raise CryptoError("Допустимые размеры ключа AES: 128, 192, 256 бит")
    timer = Timer()
    key = get_random_bytes(key_size_bits // 8)
    write_raw_key_file(key_file, key)
    log(f"Ключ AES-{key_size_bits} сгенерирован за {format_ms(timer.elapsed_ms())}")
    return key


def aes_encrypt(input_file, output_dir, key_file, key_size_bits: int) -> Path:
    timer = Timer()
    out_dir = ensure_dir(output_dir)
    key = read_raw_key_file(key_file, key_size_bits // 8)
    iv = get_random_bytes(AES.block_size)
    cipher = AES.new(key, AES.MODE_CBC, iv)

    input_file = Path(input_file)
    out_path = out_dir / (input_file.name + ".aes")
    with open_input(input_file) as fin, open_output(out_path) as fout:
        try:
            fout.write(iv)
            while True:
                chunk = fin.read(_CHUNK)
                if len(chunk) < _CHUNK:                       # конец файла: добавляем дополнение
                    fout.write(cipher.encrypt(pad(chunk, AES.block_size)))
                    break
                fout.write(cipher.encrypt(chunk))
        except BaseException:
            fout.close()
            remove_quietly(out_path)
            raise

    log(f"Шифрование AES-{key_size_bits} завершено за {format_ms(timer.elapsed_ms())}")
    log(f"  Размер файла: {format_file_size(input_file.stat().st_size)}")
    return out_path


def aes_decrypt(input_file, output_dir, key_file, key_size_bits: int) -> Path:
    timer = Timer()
    out_dir = ensure_dir(output_dir)
    key = read_raw_key_file(key_file, key_size_bits // 8)

    input_file = Path(input_file)
    total = input_file.stat().st_size if input_file.is_file() else 0
    bs = AES.block_size
    if total < 2 * bs or (total - bs) % bs != 0:
        raise CryptoError("Файл не похож на зашифрованный AES (неверный размер)")

    base = input_file.name
    if base.endswith(".aes"):
        base = base[:-4]
    out_path = out_dir / (base + ".decrypted")

    with open_input(input_file) as fin:
        iv = fin.read(bs)
        cipher = AES.new(key, AES.MODE_CBC, iv)
        with open_output(out_path) as fout:
            try:
                remaining = total - bs
                while remaining > 0:
                    chunk = fin.read(min(_CHUNK, remaining))
                    remaining -= len(chunk)
                    plain = cipher.decrypt(chunk)
                    if remaining == 0:
                        plain = unpad(plain, bs)              # последний блок: снимаем дополнение
                    fout.write(plain)
            except ValueError:
                fout.close()
                remove_quietly(out_path)
                raise CryptoError("Не удалось расшифровать: неверный ключ или файл повреждён") from None
            except BaseException:
                fout.close()
                remove_quietly(out_path)
                raise

    log(f"Расшифровка AES-{key_size_bits} завершена за {format_ms(timer.elapsed_ms())}")
    log(f"  Размер файла: {format_file_size(out_path.stat().st_size)}")
    return out_path
