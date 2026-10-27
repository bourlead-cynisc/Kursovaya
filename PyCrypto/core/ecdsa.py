"""ECDSA (P-256/P-384/P-521) на pycryptodome.
Как и в программе на C++: подписывается SHA-256 файла, подпись — r||s (IEEE P1363),
ключи — PKCS#8 / SubjectPublicKeyInfo в DER."""
from pathlib import Path

from Crypto.Hash import SHA256
from Crypto.PublicKey import ECC
from Crypto.Signature import DSS

from .errors import CryptoError
from .formatting import format_file_size, format_ms
from .logger import log
from .sha import compute_file_sha256
from .timer import Timer

CURVES = ("P-256", "P-384", "P-521")


def ecdsa_public_key_path_for(priv_file) -> Path:
    """my.key -> my_pub.key (в той же папке)"""
    p = Path(priv_file)
    return p.with_name(p.stem + "_pub.key")


def _load_key(path, what: str):
    try:
        return ECC.import_key(Path(path).read_bytes())
    except OSError as e:
        raise CryptoError(f"Не удалось открыть {what}") from e
    except (ValueError, IndexError, TypeError) as e:
        raise CryptoError(f"Файл не является корректным ECDSA-ключом ({what})") from e


def ecdsa_generate_keys(curve: str, priv_file, pub_file) -> None:
    if curve not in CURVES:
        raise CryptoError("Неизвестная кривая")
    timer = Timer()
    key = ECC.generate(curve=curve)
    try:
        Path(priv_file).write_bytes(key.export_key(format="DER"))              # PKCS#8
        Path(pub_file).write_bytes(key.public_key().export_key(format="DER"))  # SubjectPublicKeyInfo
    except OSError as e:
        raise CryptoError(f"Не удалось сохранить ключи: {e.strerror}") from e
    log(f"Ключи ECDSA ({curve}) сгенерированы за {format_ms(timer.elapsed_ms())}")


def ecdsa_sign(input_file, priv_key_file, sig_file) -> None:
    timer = Timer()
    key = _load_key(priv_key_file, "закрытый ключ")
    if not key.has_private():
        raise CryptoError("Выбран открытый ключ, а нужен закрытый")
    d = compute_file_sha256(input_file)
    # Crypto++ подписывает дайджест как сообщение (т.е. хеширует его ещё раз) — делаем так же
    signature = DSS.new(key, "fips-186-3").sign(SHA256.new(d.digest))
    try:
        Path(sig_file).write_bytes(signature)
    except OSError as e:
        raise CryptoError("Не удалось создать файл подписи") from e
    log(f"Подпись ECDSA создана за {format_ms(timer.elapsed_ms())}")
    log(f"  Размер файла: {format_file_size(d.size)}")
    log(f"  Размер подписи: {len(signature)} байт")


def ecdsa_verify(input_file, pub_key_file, sig_file) -> bool:
    """True, если подпись верна."""
    timer = Timer()
    key = _load_key(pub_key_file, "открытый ключ")
    try:
        signature = Path(sig_file).read_bytes()
    except OSError as e:
        raise CryptoError("Не удалось открыть файл подписи") from e
    d = compute_file_sha256(input_file)
    try:
        DSS.new(key, "fips-186-3").verify(SHA256.new(d.digest), signature)
        ok = True
    except ValueError:
        ok = False
    log(("Подпись ECDSA верна" if ok else "Подпись ECDSA НЕВЕРНА") + f"! Проверка заняла {format_ms(timer.elapsed_ms())}")
    return ok
