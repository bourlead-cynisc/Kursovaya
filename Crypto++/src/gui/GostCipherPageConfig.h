#pragma once
#include <filesystem>
#include <functional>
#include <vector>
#include <QString>
#include "../core/CipherRunInfo.h"

// Описание страницы блочного шифра (Магма / Кузнечик): тексты и функции из core.
struct GostCipherPageConfig {
    QString title;
    QString subtitle;
    QString defaultKeyName;
    std::function<std::vector<uint8_t>(const std::filesystem::path& keyFile)> generateKey;
    std::function<CipherRunInfo(const std::filesystem::path& in, const std::filesystem::path& outDir,
                                const std::filesystem::path& keyFile)> encrypt;
    std::function<CipherRunInfo(const std::filesystem::path& in, const std::filesystem::path& outDir,
                                const std::filesystem::path& keyFile)> decrypt;
};
