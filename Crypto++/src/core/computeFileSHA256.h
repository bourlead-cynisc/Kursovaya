#pragma once
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <cryptopp/files.h>
#include <cryptopp/filters.h>
#include <cryptopp/sha.h>
#include "FileDigest.h"

// SHA-256 файла (потоково). Общая часть для HashFile, ECDSASign и ECDSAVerify.
inline FileDigest computeFileSHA256(const std::filesystem::path& file) {
    std::ifstream in(file, std::ios::binary);
    if (!in) throw std::runtime_error("Не удалось открыть входной файл");

    FileDigest result;
    CryptoPP::SHA256 hash;
    CryptoPP::FileSource src(in, true,
        new CryptoPP::HashFilter(hash,
            new CryptoPP::ArraySink(result.bytes.data(), result.bytes.size())));
    result.size = std::filesystem::file_size(file);
    return result;
}
