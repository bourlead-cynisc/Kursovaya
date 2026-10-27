"""Работа с файлами: ключи, предпросмотр, очистка."""
import os
from pathlib import Path

from .errors import CryptoError


def file_exists(path) -> bool:
    return Path(path).is_file()


def ensure_dir(path, what: str = "Папка результата") -> Path:
    p = Path(path)
    if not p.is_dir():
        raise CryptoError(f"{what} не существует")
    return p


def write_raw_key_file(path, key: bytes) -> None:
    try:
        Path(path).write_bytes(key)
    except OSError as e:
        raise CryptoError(f"Не удалось создать файл ключа: {e.strerror}") from e


def read_raw_key_file(path, expected_bytes: int) -> bytes:
    try:
        key = Path(path).read_bytes()
    except OSError as e:
        raise CryptoError("Не удалось открыть файл ключа") from e
    if len(key) != expected_bytes:
        raise CryptoError(f"Неверный размер ключа: нужно {expected_bytes} байт, в файле {len(key)}")
    return key


def read_file_preview(path, offset: int, count: int) -> bytes:
    try:
        with open(path, "rb") as f:
            f.seek(offset)
            return f.read(count)
    except OSError:
        return b""


def remove_quietly(path) -> None:
    try:
        os.remove(path)
    except OSError:
        pass


def open_input(path):
    try:
        return open(path, "rb")
    except OSError as e:
        raise CryptoError("Не удалось открыть входной файл") from e


def open_output(path):
    try:
        return open(path, "wb")
    except OSError as e:
        raise CryptoError("Не удалось создать выходной файл") from e
