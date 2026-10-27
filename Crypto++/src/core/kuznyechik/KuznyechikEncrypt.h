#pragma once
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include "../Timer.h"
#include "../Log.h"
#include "../formatMs.h"
#include "../formatFileSize.h"
#include "../logBinary.h"
#include "../generateRandomBytes.h"
#include "../readRawKeyFile.h"
#include "../readFilePreview.h"
#include "../removeFileQuietly.h"
#include "../cbcEncryptGeneric.h"
#include "../CipherRunInfo.h"
#include "kuznExpandKey.h"
#include "kuznEncryptBlock.h"

// Файл Кузнечика: [IV 16 байт][шифртекст CBC, PKCS#7]. Результат: <имя>.kuz
inline CipherRunInfo KuznyechikEncrypt(const std::filesystem::path& inputFile,
                                       const std::filesystem::path& outputDir,
                                       const std::filesystem::path& keyFile) {
    namespace fs = std::filesystem;
    constexpr size_t kBlock = 16;
    Timer timer;
    timer.start();

    if (!fs::is_directory(outputDir)) throw std::runtime_error("Папка результата не существует");
    const auto key = readRawKeyFile(keyFile, 32);
    uint8_t rk[10][16];
    kuznExpandKey(key.data(), rk);

    CipherRunInfo info;
    info.iv = generateRandomBytes(kBlock);

    std::ifstream in(inputFile, std::ios::binary);
    if (!in) throw std::runtime_error("Не удалось открыть входной файл");
    info.outputFile = outputDir / inputFile.filename();
    info.outputFile += ".kuz";
    std::ofstream out(info.outputFile, std::ios::binary);
    if (!out) throw std::runtime_error("Не удалось создать выходной файл");

    try {
        out.write(reinterpret_cast<const char*>(info.iv.data()), kBlock);
        cbcEncryptGeneric(in, out, kBlock, info.iv.data(),
            [&rk](const uint8_t* i, uint8_t* o) { kuznEncryptBlock(i, o, rk); });
        out.flush();
        if (!out) throw std::runtime_error("Ошибка записи выходного файла");
    } catch (...) {
        out.close();
        removeFileQuietly(info.outputFile);
        throw;
    }
    out.close();

    info.inputSize = fs::file_size(inputFile);
    info.outputSize = fs::file_size(info.outputFile);
    info.preview = readFilePreview(info.outputFile, kBlock, 32);

    Log("Шифрование Кузнечиком завершено за " + formatMs(timer.elapsedMs()));
    Log("  Размер файла: " + formatFileSize(info.inputSize));
    logBinary("  Вектор инициализации (IV)", info.iv.data(), info.iv.size());
    logBinary("  Первые байты шифртекста", info.preview.data(), info.preview.size());
    return info;
}
