"""CBC + дополнение PKCS#7 для блочного шифра с любым размером блока (8 или 16 байт)."""
from typing import BinaryIO, Callable

from .errors import CryptoError

BlockFn = Callable[[bytes], bytes]


def _xor(a: bytes, b: bytes) -> bytes:
    n = len(a)
    return (int.from_bytes(a, "big") ^ int.from_bytes(b, "big")).to_bytes(n, "big")


def cbc_encrypt_stream(fin: BinaryIO, fout: BinaryIO, bs: int, iv: bytes, encrypt_block: BlockFn) -> None:
    prev = iv
    chunk_size = bs * 4096                      # кратно размеру блока
    while True:
        buf = fin.read(chunk_size)
        full = len(buf) // bs
        out = bytearray()
        for b in range(full):
            prev = encrypt_block(_xor(buf[b * bs:(b + 1) * bs], prev))
            out += prev
        if len(buf) < chunk_size:               # конец файла: последний блок с дополнением
            rem = buf[full * bs:]
            pad = bs - len(rem)
            prev = encrypt_block(_xor(rem + bytes([pad]) * pad, prev))
            out += prev
            fout.write(out)
            return
        fout.write(out)


def cbc_decrypt_stream(fin: BinaryIO, fout: BinaryIO, bs: int, total_bytes: int, iv: bytes,
                       decrypt_block: BlockFn) -> None:
    """total_bytes — длина шифртекста (кратна bs, >= bs). CryptoError("bad padding") при неверном ключе."""
    prev = iv
    blocks_left = total_bytes // bs
    while blocks_left > 0:
        count = min(blocks_left, 4096)
        buf = fin.read(count * bs)
        if len(buf) != count * bs:
            raise CryptoError("Файл прочитан не полностью")
        out = bytearray()
        for b in range(count):
            c = buf[b * bs:(b + 1) * bs]
            plain = _xor(decrypt_block(c), prev)
            prev = c
            blocks_left -= 1
            if blocks_left == 0:                # последний блок: снимаем дополнение
                pad = plain[-1]
                if pad == 0 or pad > bs or plain[bs - pad:] != bytes([pad]) * pad:
                    raise CryptoError("bad padding")
                plain = plain[:bs - pad]
            out += plain
        fout.write(out)
