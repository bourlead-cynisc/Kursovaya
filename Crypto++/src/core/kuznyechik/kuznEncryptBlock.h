#pragma once
#include <cstdint>
#include "kuznLSX.h"

// E = X[K10] LSX[K9] ... LSX[K1]
inline void kuznEncryptBlock(const uint8_t in[16], uint8_t out[16], const uint8_t rk[10][16]) {
    uint8_t s[16], n[16];
    for (int i = 0; i < 16; ++i) s[i] = in[i];
    for (int r = 0; r < 9; ++r) {
        kuznLSX(s, rk[r], n);
        for (int i = 0; i < 16; ++i) s[i] = n[i];
    }
    for (int i = 0; i < 16; ++i) out[i] = s[i] ^ rk[9][i];
}
