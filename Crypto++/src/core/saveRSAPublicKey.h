#pragma once
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <cryptopp/files.h>
#include <cryptopp/rsa.h>
inline void saveRSAPublicKey(const std::filesystem::path& file, const CryptoPP::RSA::PublicKey& key) {
    std::ofstream out(file, std::ios::binary);
    if (!out) throw std::runtime_error("Не удалось создать файл ключа");
    CryptoPP::FileSink sink(out);
    key.Save(sink);
    out.flush();
    if (!out) throw std::runtime_error("Ошибка записи файла ключа");
}
