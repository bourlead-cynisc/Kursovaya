#pragma once
#include <filesystem>
#include <string>
#include "Timer.h"
#include "Log.h"
#include "formatMs.h"
#include "formatFileSize.h"
#include "pathToUtf8.h"
#include "logBinary.h"
#include "toBinarySection.h"
#include "computeFileSHA256.h"

// Считает SHA-256 и возвращает готовый текст результата (хеш — в двоичном виде).
inline std::string HashFile(const std::filesystem::path& inputFile) {
    Timer timer;
    timer.start();

    FileDigest d = computeFileSHA256(inputFile);

    const std::string elapsed = formatMs(timer.elapsedMs());
    const std::string name = pathToUtf8(inputFile.filename());

    Log("SHA-256 вычислен за " + elapsed);
    Log("  Файл: " + name);
    Log("  Размер: " + formatFileSize(d.size));
    logBinary("  SHA-256", d.bytes.data(), d.bytes.size());

    return "Файл: " + name + "\n" +
           "Размер: " + formatFileSize(d.size) + "\n" +
           "Время: " + elapsed + "\n\n" +
           toBinarySection("SHA-256", d.bytes.data(), d.bytes.size());
}
