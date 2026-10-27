#pragma once
#include <cstdint>
#include <vector>

struct FileHash {
    std::vector<uint8_t> bytes;
    uint64_t size = 0;
};
