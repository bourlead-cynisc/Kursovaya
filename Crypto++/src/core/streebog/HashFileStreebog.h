#pragma once
#include <filesystem>
#include <string>
#include "../Timer.h"
#include "../Log.h"
#include "../formatMs.h"
#include "../formatFileSize.h"
#include "../pathToUtf8.h"
#include "../logBinary.h"
#include "../toBinarySection.h"
#include "computeFileStreebog.h"

// Хеш Стрибог-256/512 файла. Возвращает готовый текст; сам хеш — в двоичном виде.
inline std::string HashFileStreebog(const std::filesystem::path& inputFile, int bits) {
    Timer timer;
    timer.start();
    const FileHash h = computeFileStreebog(inputFile, bits);
    const std::string elapsed = formatMs(timer.elapsedMs());
    const std::string name = pathToUtf8(inputFile.filename());
    const std::string title = "Стрибог-" + std::to_string(bits);

    Log(title + " вычислен за " + elapsed);
    Log("  Файл: " + name);
    Log("  Размер: " + formatFileSize(h.size));
    logBinary("  " + title, h.bytes.data(), h.bytes.size());

    return "Файл: " + name + "\n" +
           "Размер: " + formatFileSize(h.size) + "\n" +
           "Время: " + elapsed + "\n\n" +
           toBinarySection(title, h.bytes.data(), h.bytes.size());
}
