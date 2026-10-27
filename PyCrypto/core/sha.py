"""SHA-256 файла на pycryptodome. Результат показывается в двоичном виде."""
from pathlib import Path

from Crypto.Hash import SHA256

from .errors import CryptoError
from .formatting import format_file_size, format_ms, to_binary_section
from .logger import log, log_binary
from .models import FileHash
from .timer import Timer


def compute_file_sha256(path) -> FileHash:
    h = SHA256.new()
    size = 0
    try:
        with open(path, "rb") as f:
            while chunk := f.read(1 << 20):
                h.update(chunk)
                size += len(chunk)
    except OSError as e:
        raise CryptoError("Не удалось открыть входной файл") from e
    return FileHash(h.digest(), size)


def hash_file_sha256(input_file) -> str:
    """Считает SHA-256 и возвращает готовый текст результата (хеш — в двоичном виде)."""
    timer = Timer()
    d = compute_file_sha256(input_file)
    elapsed = format_ms(timer.elapsed_ms())
    name = Path(input_file).name

    log(f"SHA-256 вычислен за {elapsed}")
    log(f"  Файл: {name}")
    log(f"  Размер: {format_file_size(d.size)}")
    log_binary("  SHA-256", d.digest)

    return (f"Файл: {name}\nРазмер: {format_file_size(d.size)}\nВремя: {elapsed}\n\n"
            + to_binary_section("SHA-256", d.digest))
