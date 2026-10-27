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
#include "../readRawKeyFile.h"
#include "../readFilePreview.h"
#include "../removeFileQuietly.h"
#include "../cbcDecryptGeneric.h"
#include "../CipherRunInfo.h"
#include "kuznExpandKey.h"
#include "kuznDecryptBlock.h"

inline CipherRunInfo KuznyechikDecrypt(const std::filesystem::path& inputFile,
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

    std::ifstream in(inputFile, std::ios::binary);
    if (!in) throw std::runtime_error("Не удалось открыть входной файл");
    const uintmax_t total = fs::file_size(inputFile);
    if (total < 2 * kBlock || (total - kBlock) % kBlock != 0)
        throw std::runtime_error("Файл не похож на зашифрованный Кузнечиком (неверный размер)");

    CipherRunInfo info;
    info.iv.resize(kBlock);
    in.read(reinterpret_cast<char*>(info.iv.data()), kBlock);
    if (in.gcount() != static_cast<std::streamsize>(kBlock)) throw std::runtime_error("Не удалось прочитать IV");

    fs::path baseName = inputFile.filename();
    if (baseName.extension() == ".kuz") baseName.replace_extension();
    baseName += ".decrypted";
    info.outputFile = outputDir / baseName;
    std::ofstream out(info.outputFile, std::ios::binary);
    if (!out) throw std::runtime_error("Не удалось создать выходной файл");

    try {
        cbcDecryptGeneric(in, out, kBlock, total - kBlock, info.iv.data(),
            [&rk](const uint8_t* i, uint8_t* o) { kuznDecryptBlock(i, o, rk); });
        out.flush();
        if (!out) throw std::runtime_error("Ошибка записи выходного файла");
    } catch (const std::runtime_error& e) {
        out.close();
        removeFileQuietly(info.outputFile);
        if (std::string(e.what()) == "bad padding")
            throw std::runtime_error("Не удалось расшифровать: неверный ключ или файл повреждён");
        throw;
    } catch (...) {
        out.close();
        removeFileQuietly(info.outputFile);
        throw;
    }
    out.close();

    info.inputSize = total;
    info.outputSize = fs::file_size(info.outputFile);
    info.preview = readFilePreview(info.outputFile, 0, 32);

    Log("Расшифровка Кузнечиком завершена за " + formatMs(timer.elapsedMs()));
    Log("  Размер файла: " + formatFileSize(info.outputSize));
    logBinary("  Вектор инициализации (IV)", info.iv.data(), info.iv.size());
    logBinary("  Первые байты открытого текста", info.preview.data(), info.preview.size());
    return info;
}
