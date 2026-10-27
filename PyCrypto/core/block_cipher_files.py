"""Файловые операции для блочных шифров ГОСТ (Магма, Кузнечик): CBC + PKCS#7, формат [IV][шифртекст]."""
from pathlib import Path
from typing import Callable

from Crypto.Random import get_random_bytes   # криптостойкий ГСЧ (pycryptodome)

from .cbc import BlockFn, cbc_decrypt_stream, cbc_encrypt_stream
from .errors import CryptoError
from .files import (ensure_dir, open_input, open_output, read_file_preview, read_raw_key_file,
                    remove_quietly, write_raw_key_file)
from .formatting import format_file_size, format_ms
from .logger import log, log_binary
from .models import CipherRunInfo
from .timer import Timer


def generate_block_cipher_key(key_file, genitive: str) -> bytes:
    """Ключ 256 бит (32 байта). genitive — название шифра в родительном падеже для журнала."""
    timer = Timer()
    key = get_random_bytes(32)
    write_raw_key_file(key_file, key)
    log(f"Ключ {genitive} (256 бит) сгенерирован за {format_ms(timer.elapsed_ms())}")
    log_binary("  Ключ", key)
    return key


def encrypt_file_cbc(input_file, output_dir, key_file, *, bs: int, ext: str, instrumental: str,
                     make_encryptor: Callable[[bytes], BlockFn]) -> CipherRunInfo:
    timer = Timer()
    out_dir = ensure_dir(output_dir)
    key = read_raw_key_file(key_file, 32)
    encrypt_block = make_encryptor(key)

    input_file = Path(input_file)
    info = CipherRunInfo(output_file=out_dir / (input_file.name + ext), iv=get_random_bytes(bs))
    with open_input(input_file) as fin, open_output(info.output_file) as fout:
        try:
            fout.write(info.iv)
            cbc_encrypt_stream(fin, fout, bs, info.iv, encrypt_block)
        except BaseException:
            fout.close()
            remove_quietly(info.output_file)
            raise

    info.input_size = input_file.stat().st_size
    info.output_size = info.output_file.stat().st_size
    info.preview = read_file_preview(info.output_file, bs, 32)

    log(f"Шифрование {instrumental} завершено за {format_ms(timer.elapsed_ms())}")
    log(f"  Размер файла: {format_file_size(info.input_size)}")
    log_binary("  Вектор инициализации (IV)", info.iv)
    log_binary("  Первые байты шифртекста", info.preview)
    return info


def decrypt_file_cbc(input_file, output_dir, key_file, *, bs: int, ext: str, instrumental: str,
                     make_decryptor: Callable[[bytes], BlockFn]) -> CipherRunInfo:
    timer = Timer()
    out_dir = ensure_dir(output_dir)
    key = read_raw_key_file(key_file, 32)
    decrypt_block = make_decryptor(key)

    input_file = Path(input_file)
    total = input_file.stat().st_size if input_file.is_file() else 0
    if total < 2 * bs or (total - bs) % bs != 0:
        raise CryptoError(f"Файл не похож на зашифрованный {instrumental} (неверный размер)")

    base = input_file.name
    if base.endswith(ext):
        base = base[:-len(ext)]
    out_path = out_dir / (base + ".decrypted")

    with open_input(input_file) as fin:
        iv = fin.read(bs)
        if len(iv) != bs:
            raise CryptoError("Не удалось прочитать IV")
        with open_output(out_path) as fout:
            try:
                cbc_decrypt_stream(fin, fout, bs, total - bs, iv, decrypt_block)
            except CryptoError as e:
                fout.close()
                remove_quietly(out_path)
                if str(e) == "bad padding":
                    raise CryptoError("Не удалось расшифровать: неверный ключ или файл повреждён") from None
                raise
            except BaseException:
                fout.close()
                remove_quietly(out_path)
                raise

    info = CipherRunInfo(output_file=out_path, iv=iv, input_size=total,
                         output_size=out_path.stat().st_size, preview=read_file_preview(out_path, 0, 32))
    log(f"Расшифровка {instrumental} завершена за {format_ms(timer.elapsed_ms())}")
    log(f"  Размер файла: {format_file_size(info.output_size)}")
    log_binary("  Вектор инициализации (IV)", info.iv)
    log_binary("  Первые байты открытого текста", info.preview)
    return info
