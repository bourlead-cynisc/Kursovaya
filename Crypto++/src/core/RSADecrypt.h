#pragma once
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <cryptopp/aes.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/filters.h>
#include <cryptopp/osrng.h>
#include <cryptopp/rsa.h>
#include "Timer.h"
#include "Log.h"
#include "formatMs.h"
#include "formatFileSize.h"
#include "loadRSAPrivateKey.h"
#include "cbcDecryptStream.h"
#include "removeFileQuietly.h"

inline std::filesystem::path RSADecrypt(const std::filesystem::path& inputFile,
                                        const std::filesystem::path& outputDir,
                                        const std::filesystem::path& privKeyFile,
                                        int keySize) {
    namespace fs = std::filesystem;
    Timer timer;
    timer.start();

    if (!fs::is_directory(outputDir)) throw std::runtime_error("Папка результата не существует");

    CryptoPP::RSA::PrivateKey privateKey;
    loadRSAPrivateKey(privKeyFile, privateKey);

    std::ifstream in(inputFile, std::ios::binary);
    if (!in) throw std::runtime_error("Не удалось открыть входной файл");

    const std::runtime_error badFormat("Файл не похож на зашифрованный RSA (неверный формат)");

    uint32_t keyLen = 0;
    in.read(reinterpret_cast<char*>(&keyLen), sizeof(keyLen));
    if (in.gcount() != sizeof(keyLen) || keyLen == 0 || keyLen > 4096) throw badFormat;

    std::string encryptedKey(keyLen, '\0');
    in.read(&encryptedKey[0], keyLen);
    if (in.gcount() != static_cast<std::streamsize>(keyLen)) throw badFormat;

    CryptoPP::byte iv[CryptoPP::AES::BLOCKSIZE];
    in.read(reinterpret_cast<char*>(iv), sizeof(iv));
    if (in.gcount() != static_cast<std::streamsize>(sizeof(iv))) throw badFormat;

    const uintmax_t header = sizeof(keyLen) + keyLen + CryptoPP::AES::BLOCKSIZE;
    const uintmax_t total = fs::file_size(inputFile);
    if (total < header + CryptoPP::AES::BLOCKSIZE || (total - header) % CryptoPP::AES::BLOCKSIZE != 0) throw badFormat;

    CryptoPP::SecByteBlock aesKey;
    try {
        CryptoPP::RSAES_OAEP_SHA_Decryptor decryptor(privateKey);
        CryptoPP::AutoSeededRandomPool rng;
        std::string aesKeyStr;
        CryptoPP::StringSource(reinterpret_cast<const CryptoPP::byte*>(encryptedKey.data()),
            encryptedKey.size(), true,
            new CryptoPP::PK_DecryptorFilter(rng, decryptor, new CryptoPP::StringSink(aesKeyStr)));
        if (aesKeyStr.size() != 32) throw badFormat;
        aesKey.Assign(reinterpret_cast<const CryptoPP::byte*>(aesKeyStr.data()), aesKeyStr.size());
    } catch (const CryptoPP::Exception&) {
        throw std::runtime_error("Не удалось расшифровать ключ: выбран не тот закрытый ключ");
    }

    fs::path baseName = inputFile.filename();
    if (baseName.extension() == ".rsa") baseName.replace_extension();
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
        throw std::runtime_error("Не удалось расшифровать: файл повреждён");
    } catch (...) {
        out.close();
        removeFileQuietly(outputFile);
        throw;
    }
    out.close();

    Log("Расшифровка RSA " + std::to_string(keySize) + "-bit завершена за " + formatMs(timer.elapsedMs()));
    Log("  Размер файла: " + formatFileSize(fs::file_size(outputFile)));
    return outputFile;
}
