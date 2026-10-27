#pragma once
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <cryptopp/files.h>
#include <cryptopp/rsa.h>
inline void loadRSAPublicKey(const std::filesystem::path& file, CryptoPP::RSA::PublicKey& key) {
    std::ifstream in(file, std::ios::binary);
    if (!in) throw std::runtime_error("Не удалось открыть файл ключа");
    CryptoPP::FileSource src(in, true);
    key.Load(src);
}
