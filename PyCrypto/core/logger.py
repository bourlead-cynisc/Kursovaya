"""Журнал. Ядро пишет через log(); куда именно выводить — решает интерфейс (set_log_sink)."""
import time
from typing import Callable, Optional

from .formatting import to_binary_string

_sink: Optional[Callable[[str], None]] = None


def set_log_sink(sink: Optional[Callable[[str], None]]) -> None:
    global _sink
    _sink = sink


def log(text: str) -> None:
    if _sink is not None:
        _sink(f"[{time.strftime('%H:%M:%S')}] {text}")


def log_binary(title: str, data: bytes, bytes_per_line: int = 4) -> None:
    """Двоичные данные в журнал: заголовок и по строке на каждые bytes_per_line байт."""
    log(f"{title} ({len(data) * 8} бит):")
    for row in to_binary_string(data, bytes_per_line).split("\n"):
        if row:
            log("  " + row)
