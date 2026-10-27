#pragma once
#include <cstdint>
#include "../loadBE32.h"

// 256-битный ключ -> 32 раундовых ключа: K1..K8, K1..K8, K1..K8, K8..K1 (RFC 8891, п. 4.3).
inline void magmaExpandKey(const uint8_t key[32], uint32_t rk[32]) {
    uint32_t k[8];
    for (int i = 0; i < 8; ++i) k[i] = loadBE32(key + 4 * i);
    for (int i = 0; i < 24; ++i) rk[i] = k[i % 8];
    for (int i = 0; i < 8; ++i) rk[24 + i] = k[7 - i];
}
