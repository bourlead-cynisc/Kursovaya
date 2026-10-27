"""Форматирование: размеры, время и — главное — двоичный вывод данных."""


def format_file_size(size: int) -> str:
    units = ("B", "KB", "MB", "GB")
    value = float(size)
    unit = 0
    while value >= 1024 and unit < 3:
        value /= 1024
        unit += 1
    return f"{value:.2f} {units[unit]}"


def format_ms(ms: float) -> str:
    return f"{ms:.2f} ms"


def to_binary_string(data: bytes, bytes_per_line: int = 8) -> str:
    """Данные -> двоичная запись (0/1). Байты через пробел, строки по bytes_per_line байт."""
    rows = []
    for i in range(0, len(data), bytes_per_line):
        rows.append(" ".join(f"{b:08b}" for b in data[i:i + bytes_per_line]))
    return "\n".join(rows)


def to_binary_section(title: str, data: bytes, bytes_per_line: int = 8) -> str:
    """Блок для вывода: заголовок с длиной в битах + данные в двоичном виде."""
    return f"{title} ({len(data) * 8} бит):\n{to_binary_string(data, bytes_per_line)}\n"
