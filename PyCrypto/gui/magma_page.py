from core.magma import magma_decrypt, magma_encrypt, magma_generate_key

from .gost_cipher_page import GostCipherPageConfig, create_gost_cipher_page


def create_magma_page(ctx):
    return create_gost_cipher_page(ctx, GostCipherPageConfig(
        title="Магма",
        subtitle="ГОСТ Р 34.12-2015: блок 64 бита, ключ 256 бит, 32 раунда. Режим CBC, дополнение PKCS#7. "
                 "Реализовано вручную (таблица замен из RFC 8891).",
        default_key_name="magma.key",
        generate_key=magma_generate_key, encrypt=magma_encrypt, decrypt=magma_decrypt))
