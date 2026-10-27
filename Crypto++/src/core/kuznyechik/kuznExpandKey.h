#pragma once
#include <cstdint>
#include <cstring>
#include "kuznLSX.h"

// 256-битный ключ -> 10 раундовых ключей по 128 бит (RFC 7801, п. 4.4).
inline void kuznExpandKey(const uint8_t key[32], uint8_t rk[10][16]) {
    const KuznyechikTables& t = kuznTables();
    uint8_t a1[16], a0[16], tmp[16];
    std::memcpy(a1, key, 16);
    std::memcpy(a0, key + 16, 16);
    std::memcpy(rk[0], a1, 16);
    std::memcpy(rk[1], a0, 16);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 8; ++j) {                 // F[C](a1,a0) = (LSX[C](a1) xor a0, a1)
            kuznLSX(a1, t.c[8 * i + j], tmp);
            for (int b = 0; b < 16; ++b) tmp[b] ^= a0[b];
            std::memcpy(a0, a1, 16);
            std::memcpy(a1, tmp, 16);
        }
        std::memcpy(rk[2 * i + 2], a1, 16);
        std::memcpy(rk[2 * i + 3], a0, 16);
    }
}
