#pragma once
#include <array>
#include <cstdint>
#include <cryptopp/sha.h>

struct FileDigest {
    std::array<CryptoPP::byte, CryptoPP::SHA256::DIGESTSIZE> bytes{};
    uint64_t size = 0;
};
