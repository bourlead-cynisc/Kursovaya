"""Стрибог — ГОСТ Р 34.11-2012 (RFC 6986), реализация вручную, хеш 256 или 512 бит.
Таблицы (Pi, матрица A, Tau, константы C) — core/gost_pi.py и core/streebog_tables.py.

Все 512-битные значения — Python-числа в little-endian (байт 0 — младший), поэтому байты
сообщения ложатся в число «как есть». Хеш выдаётся в порядке памяти (как у OpenSSL/gostsum);
в RFC 6986 те же значения записаны как большое число, то есть в обратном порядке байт."""
import struct
from pathlib import Path

from .errors import CryptoError
from .formatting import format_file_size, format_ms, to_binary_section
from .gost_pi import PI
from .logger import log, log_binary
from .models import FileHash
from .streebog_tables import A, C_HEX, TAU
from .timer import Timer

MASK512 = (1 << 512) - 1


def streebog_l(w: int) -> int:
    """Линейное преобразование l над 64-битным вектором: бит i выбирает строку A[63 - i]."""
    r = 0
    for i in range(64):
        if (w >> i) & 1:
            r ^= A[63 - i]
    return r


class _Tables:
    def __init__(self) -> None:
        # C[i]: в стандарте записаны старшим байтом вперёд -> разворачиваем в little-endian
        self.c = [int.from_bytes(bytes.fromhex(h)[::-1], "little") for h in C_HEX]
        # t[pos][b] = l(b << 8*pos): преобразование L по байтам (подстановка S применяется отдельно)
        self.t = [[streebog_l(b << (8 * pos)) for b in range(256)] for pos in range(8)]


_tables = None


def tables() -> _Tables:
    global _tables
    if _tables is None:
        _tables = _Tables()
    return _tables


def streebog_lps(x: int) -> int:
    """LPS(x) = L(P(S(x))) для 512-битного числа."""
    t = tables().t
    b = x.to_bytes(64, "little")
    p = bytes([b[i] for i in TAU]).translate(PI)       # P (транспонирование), S (подстановка Pi)
    t0, t1, t2, t3, t4, t5, t6, t7 = t
    words = [t0[p[o]] ^ t1[p[o + 1]] ^ t2[p[o + 2]] ^ t3[p[o + 3]] ^
             t4[p[o + 4]] ^ t5[p[o + 5]] ^ t6[p[o + 6]] ^ t7[p[o + 7]] for o in range(0, 64, 8)]   # L
    return int.from_bytes(struct.pack("<8Q", *words), "little")


def streebog_e(k: int, m: int) -> int:
    """E(K, m) = X[K13] LPSX[K12] ... LPSX[K1](m), K[i] = LPS(K[i-1] xor C[i-1])."""
    c = tables().c
    s = m
    for ci in c:
        s = streebog_lps(s ^ k)
        k = streebog_lps(k ^ ci)
    return s ^ k


def streebog_g(n: int, h: int, m: int) -> int:
    """Функция сжатия g_N(h, m) = E(LPS(h xor N), m) xor h xor m."""
    return streebog_e(streebog_lps(h ^ n), m) ^ h ^ m


class StreebogHasher:
    """Потоковый хешер: update() можно вызывать кусками любой длины, digest() — в конце."""

    def __init__(self, bits: int = 512) -> None:
        if bits not in (256, 512):
            raise CryptoError("Стрибог: длина хеша 256 или 512 бит")
        self.bits = bits
        self.h = int.from_bytes(b"\x01" * 64, "little") if bits == 256 else 0   # IV
        self.n = 0
        self.sigma = 0
        self.buf = b""

    def update(self, data: bytes) -> None:
        self.buf += data
        # полные блоки обрабатываем сразу: по стандарту после них всегда идёт добивающий блок
        while len(self.buf) >= 64:
            m = int.from_bytes(self.buf[:64], "little")
            self.buf = self.buf[64:]
            self.h = streebog_g(self.n, self.h, m)
            self.n = (self.n + 512) & MASK512
            self.sigma = (self.sigma + m) & MASK512

    def digest(self) -> bytes:
        m = int.from_bytes(self.buf + b"\x01", "little")        # дополнение: 0^(511-|M|) || 1 || M
        h = streebog_g(self.n, self.h, m)
        n = (self.n + len(self.buf) * 8) & MASK512
        sigma = (self.sigma + m) & MASK512
        h = streebog_g(0, h, n)
        h = streebog_g(0, h, sigma)
        out = h.to_bytes(64, "little")
        return out if self.bits == 512 else out[32:]            # MSB_256


def streebog_digest(data: bytes, bits: int = 512) -> bytes:
    hasher = StreebogHasher(bits)
    hasher.update(data)
    return hasher.digest()


def compute_file_streebog(path, bits: int) -> FileHash:
    hasher = StreebogHasher(bits)
    size = 0
    try:
        with open(path, "rb") as f:
            while chunk := f.read(1 << 20):
                hasher.update(chunk)
                size += len(chunk)
    except OSError as e:
        raise CryptoError("Не удалось открыть входной файл") from e
    return FileHash(hasher.digest(), size)


def hash_file_streebog(input_file, bits: int) -> str:
    """Хеш Стрибог-256/512 файла. Возвращает готовый текст; сам хеш — в двоичном виде."""
    timer = Timer()
    h = compute_file_streebog(input_file, bits)
    elapsed = format_ms(timer.elapsed_ms())
    name = Path(input_file).name
    title = f"Стрибог-{bits}"

    log(f"{title} вычислен за {elapsed}")
    log(f"  Файл: {name}")
    log(f"  Размер: {format_file_size(h.size)}")
    log_binary(f"  {title}", h.digest)

    return (f"Файл: {name}\nРазмер: {format_file_size(h.size)}\nВремя: {elapsed}\n\n"
            + to_binary_section(title, h.digest))
