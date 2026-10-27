#pragma once
#include "createGostCipherPage.h"
#include "../core/magma/MagmaGenerateKey.h"
#include "../core/magma/MagmaEncrypt.h"
#include "../core/magma/MagmaDecrypt.h"

inline QWidget* createMagmaPage(AppContext* ctx) {
    GostCipherPageConfig cfg;
    cfg.title = QStringLiteral("Магма");
    cfg.subtitle = QStringLiteral("ГОСТ Р 34.12-2015: блок 64 бита, ключ 256 бит, 32 раунда. Режим CBC, дополнение PKCS#7.");
    cfg.defaultKeyName = QStringLiteral("magma.key");
    cfg.generateKey = [](const std::filesystem::path& k) { return MagmaGenerateKey(k); };
    cfg.encrypt = [](const std::filesystem::path& i, const std::filesystem::path& o, const std::filesystem::path& k) { return MagmaEncrypt(i, o, k); };
    cfg.decrypt = [](const std::filesystem::path& i, const std::filesystem::path& o, const std::filesystem::path& k) { return MagmaDecrypt(i, o, k); };
    return createGostCipherPage(ctx, cfg);
}
