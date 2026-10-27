#pragma once
#include <cstdint>
#include <vector>
#include <cryptopp/osrng.h>

// Криптостойкие случайные байты (Crypto++ AutoSeededRandomPool).
inline std::vector<uint8_t> generateRandomBytes(size_t count) {
    CryptoPP::AutoSeededRandomPool rng;
    std::vector<uint8_t> v(count);
    if (count) rng.GenerateBlock(v.data(), v.size());
    return v;
}
