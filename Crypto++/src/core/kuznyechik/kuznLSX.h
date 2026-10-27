#pragma once
#include <cstdint>
#include "kuznTables.h"

// LSX[k](a) = L(S(a xor k)); результат пишется в out.
inline void kuznLSX(const uint8_t a[16], const uint8_t k[16], uint8_t out[16]) {
    const KuznyechikTables& t = kuznTables();
    uint8_t acc[16] = {0};
    for (int i = 0; i < 16; ++i) {
        const uint8_t v = t.pi[uint8_t(a[i] ^ k[i])];
        const uint8_t* row = t.encL[i][v];
        for (int j = 0; j < 16; ++j) acc[j] ^= row[j];
    }
    for (int j = 0; j < 16; ++j) out[j] = acc[j];
}
