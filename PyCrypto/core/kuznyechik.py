"""Кузнечик — ГОСТ Р 34.12-2015 (RFC 7801), реализация вручную: блок 128 бит, ключ 256 бит, 10 раундов.
Подстановка Pi — core/gost_pi.py. Линейное преобразование L задаётся коэффициентами в GF(2^8)
с неприводимым многочленом x^8+x^7+x^6+x+1. Режим CBC, файлы *.kuz.

Порядок байт: b[0] = a_15 (старший) ... b[15] = a_0 — как в записи строки слева направо."""
from .block_cipher_files import decrypt_file_cbc, encrypt_file_cbc, generate_block_cipher_key
from .gost_pi import PI
from .models import CipherRunInfo

# Коэффициенты l при a_15, a_14, ..., a_0 (RFC 7801, п. 4.2; опечатка "a_15" вместо "a_14" в тексте
# RFC исправлена по errata EID 4660).
L_COEF = (148, 32, 133, 16, 194, 192, 1, 251, 1, 192, 194, 16, 133, 32, 148, 1)

PI_INV = bytes(sorted(range(256), key=lambda i: PI[i]))   # PI_INV[PI[i]] = i


def gf_mul(a: int, b: int) -> int:
    """Умножение в GF(2)[x]/(x^8+x^7+x^6+x+1)."""
    r = 0
    while b:
        if b & 1:
            r ^= a
        carry = a & 0x80
        a = (a << 1) & 0xFF
        if carry:
            a ^= 0xC3                        # 0x1C3 без старшего бита
        b >>= 1
    return r


def linear(a) -> int:
    """l(a_15,...,a_0)"""
    r = 0
    for x, c in zip(a, L_COEF):
        r ^= gf_mul(x, c)
    return r


def transform_r(a: list) -> list:
    """R(a_15||...||a_0) = l(a_15,...,a_0)||a_15||...||a_1"""
    return [linear(a)] + a[:15]


def transform_r_inv(a: list) -> list:
    """R^-1(a_15||...||a_0) = a_14||...||a_0||l(a_14,...,a_0,a_15)"""
    return a[1:] + [linear(a[1:] + a[:1])]


def transform_l(a: list) -> list:
    for _ in range(16):
        a = transform_r(a)
    return a


def transform_l_inv(a: list) -> list:
    for _ in range(16):
        a = transform_r_inv(a)
    return a


class _Tables:
    """Таблицы для быстрой работы, построенные из определений выше (один раз)."""
    def __init__(self) -> None:
        # L линейно над GF(2^8): L(v в позиции i) = v * L(единичный вектор i) — хватает 16 вычислений L
        enc_unit = [transform_l([1 if j == i else 0 for j in range(16)]) for i in range(16)]
        dec_unit = [transform_l_inv([1 if j == i else 0 for j in range(16)]) for i in range(16)]
        self.enc_l = [[int.from_bytes(bytes(gf_mul(v, x) for x in enc_unit[i]), "big") for v in range(256)]
                      for i in range(16)]
        self.dec_l = [[int.from_bytes(bytes(gf_mul(v, x) for x in dec_unit[i]), "big") for v in range(256)]
                      for i in range(16)]
        # раундовые константы C_i = L(Vec128(i)), i = 1..32
        self.c = [int.from_bytes(bytes(transform_l([0] * 15 + [i])), "big") for i in range(1, 33)]


_tables = None


def tables() -> _Tables:
    global _tables
    if _tables is None:
        _tables = _Tables()
    return _tables


def lsx(a: int, k: int) -> int:
    """LSX[k](a) = L(S(a xor k)); a, k — 128-битные числа (big-endian)."""
    enc_l = tables().enc_l
    s = (a ^ k).to_bytes(16, "big").translate(PI)
    acc = 0
    for i in range(16):
        acc ^= enc_l[i][s[i]]
    return acc


def kuz_expand_key(key: bytes) -> list:
    """256-битный ключ -> 10 раундовых ключей по 128 бит (RFC 7801, п. 4.4)."""
    c = tables().c
    a1 = int.from_bytes(key[:16], "big")
    a0 = int.from_bytes(key[16:], "big")
    rk = [a1, a0]
    for i in range(4):
        for j in range(8):                   # F[C](a1,a0) = (LSX[C](a1) xor a0, a1)
            a1, a0 = lsx(a1, c[8 * i + j]) ^ a0, a1
        rk += [a1, a0]
    return rk


def kuz_encrypt_block(block: bytes, rk: list) -> bytes:
    """E = X[K10] LSX[K9] ... LSX[K1]"""
    s = int.from_bytes(block, "big")
    for r in range(9):
        s = lsx(s, rk[r])
    return (s ^ rk[9]).to_bytes(16, "big")


def kuz_decrypt_block(block: bytes, rk: list) -> bytes:
    """D = X[K1] L^-1 S^-1 X[K2] ... L^-1 S^-1 X[K10]"""
    dec_l = tables().dec_l
    s = int.from_bytes(block, "big") ^ rk[9]
    for r in range(8, -1, -1):
        b = s.to_bytes(16, "big")
        acc = 0                              # L^-1
        for i in range(16):
            acc ^= dec_l[i][b[i]]
        s = int.from_bytes(acc.to_bytes(16, "big").translate(PI_INV), "big") ^ rk[r]   # S^-1, затем X[K]
    return s.to_bytes(16, "big")


def kuznyechik_generate_key(key_file) -> bytes:
    return generate_block_cipher_key(key_file, "Кузнечика")


def kuznyechik_encrypt(input_file, output_dir, key_file) -> CipherRunInfo:
    def make(key):
        rk = kuz_expand_key(key)
        return lambda blk: kuz_encrypt_block(blk, rk)
    return encrypt_file_cbc(input_file, output_dir, key_file, bs=16, ext=".kuz",
                            instrumental="Кузнечиком", make_encryptor=make)


def kuznyechik_decrypt(input_file, output_dir, key_file) -> CipherRunInfo:
    def make(key):
        rk = kuz_expand_key(key)
        return lambda blk: kuz_decrypt_block(blk, rk)
    return decrypt_file_cbc(input_file, output_dir, key_file, bs=16, ext=".kuz",
                            instrumental="Кузнечиком", make_decryptor=make)
