from dataclasses import dataclass
from pathlib import Path


@dataclass
class CipherRunInfo:
    """Результат операции Магмы/Кузнечика: путь результата и данные для показа в двоичном виде."""
    output_file: Path
    iv: bytes = b""
    preview: bytes = b""
    input_size: int = 0
    output_size: int = 0


@dataclass
class FileHash:
    digest: bytes
    size: int
