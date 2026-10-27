#pragma once
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <cryptopp/files.h>
#include <cryptopp/rsa.h>
inline void loadRSAPrivateKey(const std::filesystem::path& file, CryptoPP::RSA::PrivateKey& key) {
    std::ifstream in(file, std::ios::binary);
    if (!in) throw std::runtime_error("Не удалось открыть файл ключа");
    CryptoPP::FileSource src(in, true);
    key.Load(src);
}
