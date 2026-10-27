from core.kuznyechik import kuznyechik_decrypt, kuznyechik_encrypt, kuznyechik_generate_key

from .gost_cipher_page import GostCipherPageConfig, create_gost_cipher_page


def create_kuznyechik_page(ctx):
    return create_gost_cipher_page(ctx, GostCipherPageConfig(
        title="Кузнечик",
        subtitle="ГОСТ Р 34.12-2015: блок 128 бит, ключ 256 бит, 10 раундов. Режим CBC, дополнение PKCS#7. "
                 "Реализовано вручную (подстановка π и матрица L из RFC 7801).",
        default_key_name="kuznyechik.key",
        generate_key=kuznyechik_generate_key, encrypt=kuznyechik_encrypt, decrypt=kuznyechik_decrypt))
