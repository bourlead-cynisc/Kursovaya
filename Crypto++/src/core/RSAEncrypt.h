#pragma once
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/osrng.h>
#include <cryptopp/rsa.h>
#include "Timer.h"
#include "Log.h"
#include "formatMs.h"
#include "formatFileSize.h"
#include "loadRSAPublicKey.h"
#include "cbcEncryptStream.h"
#include "removeFileQuietly.h"

// Гибридное шифрование. Формат:
// [uint32 длина][RSA-OAEP(AES-256 ключ)][IV 16 байт][AES-CBC шифртекст]
inline std::filesystem::path RSAEncrypt(const std::filesystem::path& inputFile,
                                        const std::filesystem::path& outputDir,
                                        const std::filesystem::path& pubKeyFile,
                                        int keySize) {
    namespace fs = std::filesystem;
    Timer timer;
    timer.start();

    if (!fs::is_directory(outputDir)) throw std::runtime_error("Папка результата не существует");

    CryptoPP::RSA::PublicKey publicKey;
    loadRSAPublicKey(pubKeyFile, publicKey);

    CryptoPP::AutoSeededRandomPool rng;
    CryptoPP::SecByteBlock aesKey(32);
    rng.GenerateBlock(aesKey, aesKey.size());
    CryptoPP::byte iv[CryptoPP::AES::BLOCKSIZE];
    rng.GenerateBlock(iv, sizeof(iv));

    CryptoPP::RSAES_OAEP_SHA_Encryptor encryptor(publicKey);
    std::string encryptedKey;
    CryptoPP::StringSource(aesKey, aesKey.size(), true,
        new CryptoPP::PK_EncryptorFilter(rng, encryptor, new CryptoPP::StringSink(encryptedKey)));

    std::ifstream in(inputFile, std::ios::binary);
    if (!in) throw std::runtime_error("Не удалось открыть входной файл");

    fs::path outputFile = outputDir / inputFile.filename();
    outputFile += ".rsa";
    std::ofstream out(outputFile, std::ios::binary);
    if (!out) throw std::runtime_error("Не удалось создать выходной файл");

    try {
        const uint32_t keyLen = static_cast<uint32_t>(encryptedKey.size());
        out.write(reinterpret_cast<const char*>(&keyLen), sizeof(keyLen));
        out.write(encryptedKey.data(), static_cast<std::streamsize>(encryptedKey.size()));
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

    Log("Шифрование RSA " + std::to_string(keySize) + "-bit завершено за " + formatMs(timer.elapsedMs()));
    Log("  Размер файла: " + formatFileSize(fs::file_size(inputFile)));
    return outputFile;
}
