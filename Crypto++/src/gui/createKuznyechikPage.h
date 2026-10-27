#pragma once
#include "createGostCipherPage.h"
#include "../core/kuznyechik/KuznyechikGenerateKey.h"
#include "../core/kuznyechik/KuznyechikEncrypt.h"
#include "../core/kuznyechik/KuznyechikDecrypt.h"

inline QWidget* createKuznyechikPage(AppContext* ctx) {
    GostCipherPageConfig cfg;
    cfg.title = QStringLiteral("Кузнечик");
    cfg.subtitle = QStringLiteral("ГОСТ Р 34.12-2015: блок 128 бит, ключ 256 бит, 10 раундов. Режим CBC, дополнение PKCS#7.");
    cfg.defaultKeyName = QStringLiteral("kuznyechik.key");
    cfg.generateKey = [](const std::filesystem::path& k) { return KuznyechikGenerateKey(k); };
    cfg.encrypt = [](const std::filesystem::path& i, const std::filesystem::path& o, const std::filesystem::path& k) { return KuznyechikEncrypt(i, o, k); };
    cfg.decrypt = [](const std::filesystem::path& i, const std::filesystem::path& o, const std::filesystem::path& k) { return KuznyechikDecrypt(i, o, k); };
    return createGostCipherPage(ctx, cfg);
}
