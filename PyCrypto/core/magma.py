"""Магма — ГОСТ Р 34.12-2015 (RFC 8891), реализация вручную: блок 64 бита, ключ 256 бит, 32 раунда.
Таблица замен — core/magma_sbox.py. Режим CBC, файлы *.magma."""
from .block_cipher_files import decrypt_file_cbc, encrypt_file_cbc, generate_block_cipher_key
from .magma_sbox import MAGMA_SBOX
from .models import CipherRunInfo

MASK32 = 0xFFFFFFFF


def magma_t_spec(a: int) -> int:
    """t(a) = Pi_7(a_7)||...||Pi_0(a_0): подстановка по 4-битным частям (прямо по определению)."""
    r = 0
    for i in range(8):
        r |= MAGMA_SBOX[i][(a >> (4 * i)) & 0xF] << (4 * i)
    return r


# То же преобразование t, но по байтам (4 обращения к таблице вместо 8) — для скорости.
_T_TABLES = tuple(
    tuple((MAGMA_SBOX[2 * k + 1][b >> 4] << 4 | MAGMA_SBOX[2 * k][b & 15]) << (8 * k) for b in range(256))
    for k in range(4)
)


def magma_t(a: int) -> int:
    t = _T_TABLES
    return t[0][a & 0xFF] | t[1][(a >> 8) & 0xFF] | t[2][(a >> 16) & 0xFF] | t[3][(a >> 24) & 0xFF]


def magma_g(k: int, a: int) -> int:
    """g[k](a) = t(a + k mod 2^32) <<< 11"""
    t = magma_t((a + k) & MASK32)
    return ((t << 11) | (t >> 21)) & MASK32


def magma_expand_key(key: bytes) -> list:
    """256-битный ключ -> 32 раундовых ключа: K1..K8, K1..K8, K1..K8, K8..K1 (RFC 8891, п. 4.3)."""
    k = [int.from_bytes(key[4 * i:4 * i + 4], "big") for i in range(8)]
    return k * 3 + k[::-1]


def magma_encrypt_block(block: bytes, rk: list) -> bytes:
    # раунды развёрнуты прямо в цикле (без вызовов magma_g) — в чистом Python это заметно быстрее
    t0, t1, t2, t3 = _T_TABLES
    a1 = int.from_bytes(block[:4], "big")
    a0 = int.from_bytes(block[4:], "big")
    for k in rk[:31]:                         # G[K_i](a1, a0) = (a0, g[K_i](a0) xor a1)
        x = (a0 + k) & MASK32
        t = t0[x & 0xFF] | t1[(x >> 8) & 0xFF] | t2[(x >> 16) & 0xFF] | t3[x >> 24]
        a1, a0 = a0, (((t << 11) | (t >> 21)) & MASK32) ^ a1
    hi = magma_g(rk[31], a0) ^ a1             # G*[K_32]
    return hi.to_bytes(4, "big") + a0.to_bytes(4, "big")


def magma_decrypt_block(block: bytes, rk: list) -> bytes:
    t0, t1, t2, t3 = _T_TABLES
    a1 = int.from_bytes(block[:4], "big")
    a0 = int.from_bytes(block[4:], "big")
    for k in rk[31:0:-1]:
        x = (a0 + k) & MASK32
        t = t0[x & 0xFF] | t1[(x >> 8) & 0xFF] | t2[(x >> 16) & 0xFF] | t3[x >> 24]
        a1, a0 = a0, (((t << 11) | (t >> 21)) & MASK32) ^ a1
    hi = magma_g(rk[0], a0) ^ a1
    return hi.to_bytes(4, "big") + a0.to_bytes(4, "big")


def magma_generate_key(key_file) -> bytes:
    return generate_block_cipher_key(key_file, "Магмы")


def magma_encrypt(input_file, output_dir, key_file) -> CipherRunInfo:
    def make(key):
        rk = magma_expand_key(key)
        return lambda blk: magma_encrypt_block(blk, rk)
    return encrypt_file_cbc(input_file, output_dir, key_file, bs=8, ext=".magma",
                            instrumental="Магмой", make_encryptor=make)


def magma_decrypt(input_file, output_dir, key_file) -> CipherRunInfo:
    def make(key):
        rk = magma_expand_key(key)
        return lambda blk: magma_decrypt_block(blk, rk)
    return decrypt_file_cbc(input_file, output_dir, key_file, bs=8, ext=".magma",
                            instrumental="Магмой", make_decryptor=make)
