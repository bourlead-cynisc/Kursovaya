#pragma once
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <cryptopp/aes.h>
#include <cryptopp/cryptlib.h>
#include "Timer.h"
#include "Log.h"
#include "formatMs.h"
#include "formatFileSize.h"
#include "readAESKeyFile.h"
#include "cbcDecryptStream.h"
#include "removeFileQuietly.h"

inline std::filesystem::path AESDecrypt(const std::filesystem::path& inputFile,
                                        const std::filesystem::path& outputDir,
                                        const std::filesystem::path& keyFile,
                                        int keySizeBits) {
    namespace fs = std::filesystem;
    Timer timer;
    timer.start();

    if (!fs::is_directory(outputDir)) throw std::runtime_error("Папка результата не существует");
    CryptoPP::SecByteBlock aesKey = readAESKeyFile(keyFile, keySizeBits);

    std::ifstream in(inputFile, std::ios::binary);
    if (!in) throw std::runtime_error("Не удалось открыть входной файл");

    const uintmax_t total = fs::file_size(inputFile);
    if (total < 2 * CryptoPP::AES::BLOCKSIZE || (total - CryptoPP::AES::BLOCKSIZE) % CryptoPP::AES::BLOCKSIZE != 0)
        throw std::runtime_error("Файл не похож на зашифрованный AES (неверный размер)");

    CryptoPP::byte iv[CryptoPP::AES::BLOCKSIZE];
    in.read(reinterpret_cast<char*>(iv), sizeof(iv));
    if (in.gcount() != static_cast<std::streamsize>(sizeof(iv)))
        throw std::runtime_error("Не удалось прочитать IV");

    fs::path baseName = inputFile.filename();
    if (baseName.extension() == ".aes") baseName.replace_extension();
    baseName += ".decrypted";
    fs::path outputFile = outputDir / baseName;

    std::ofstream out(outputFile, std::ios::binary);
    if (!out) throw std::runtime_error("Не удалось создать выходной файл");

    try {
        cbcDecryptStream(in, out, aesKey, iv);
        out.flush();
        if (!out) throw std::runtime_error("Ошибка записи выходного файла");
    } catch (const CryptoPP::Exception&) {
        out.close();
        removeFileQuietly(outputFile);
        throw std::runtime_error("Не удалось расшифровать: неверный ключ или файл повреждён");
    } catch (...) {
        out.close();
        removeFileQuietly(outputFile);
        throw;
    }
    out.close();

    Log("Расшифровка AES-" + std::to_string(keySizeBits) + " завершена за " + formatMs(timer.elapsedMs()));
    Log("  Размер файла: " + formatFileSize(fs::file_size(outputFile)));
    return outputFile;
}
