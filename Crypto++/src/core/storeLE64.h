#pragma once
#include <cstdint>

inline void storeLE64(uint8_t* p, uint64_t v) {
    for (int i = 0; i < 8; ++i) { p[i] = uint8_t(v); v >>= 8; }
}
