#pragma once
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <cryptopp/aes.h>
#include <cryptopp/osrng.h>
#include "Timer.h"
#include "Log.h"
#include "formatMs.h"
#include "formatFileSize.h"
#include "readAESKeyFile.h"
#include "cbcEncryptStream.h"
#include "removeFileQuietly.h"

// Формат файла: [IV 16 байт][AES-CBC шифртекст]. Возвращает путь к результату.
inline std::filesystem::path AESEncrypt(const std::filesystem::path& inputFile,
                                        const std::filesystem::path& outputDir,
                                        const std::filesystem::path& keyFile,
                                        int keySizeBits) {
    namespace fs = std::filesystem;
    Timer timer;
    timer.start();

    if (!fs::is_directory(outputDir)) throw std::runtime_error("Папка результата не существует");
    CryptoPP::SecByteBlock aesKey = readAESKeyFile(keyFile, keySizeBits);

    CryptoPP::AutoSeededRandomPool rng;
    CryptoPP::byte iv[CryptoPP::AES::BLOCKSIZE];
    rng.GenerateBlock(iv, sizeof(iv));

    std::ifstream in(inputFile, std::ios::binary);
    if (!in) throw std::runtime_error("Не удалось открыть входной файл");

    fs::path outputFile = outputDir / inputFile.filename();
    outputFile += ".aes";
    std::ofstream out(outputFile, std::ios::binary);
    if (!out) throw std::runtime_error("Не удалось создать выходной файл");

    try {
        out.write(reinterpret_cast<const char*>(iv), sizeof(iv));
        cbcEncryptStream(in, out, aesKey, iv);
        out.flush();
        if (!out) throw std::runtime_error("Ошибка записи выходного файла");
    } catch (...) {
        out.close();
        removeFileQuietly(outputFile);
        throw;
    }
    out.close();

    Log("Шифрование AES-" + std::to_string(keySizeBits) + " завершено за " + formatMs(timer.elapsedMs()));
    Log("  Размер файла: " + formatFileSize(fs::file_size(inputFile)));
    return outputFile;
}
