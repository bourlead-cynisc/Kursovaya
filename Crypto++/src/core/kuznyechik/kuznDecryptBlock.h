#pragma once
#include <cstdint>
#include "kuznTables.h"

// D = X[K1] L^-1 S^-1 X[K2] ... L^-1 S^-1 X[K10]  (порядок действий как в примере RFC 7801, п. 5.6)
inline void kuznDecryptBlock(const uint8_t in[16], uint8_t out[16], const uint8_t rk[10][16]) {
    const KuznyechikTables& t = kuznTables();
    uint8_t s[16], n[16];
    for (int i = 0; i < 16; ++i) s[i] = in[i] ^ rk[9][i];
    for (int r = 8; r >= 0; --r) {
        uint8_t acc[16] = {0};                         // L^-1
        for (int i = 0; i < 16; ++i) {
            const uint8_t* row = t.decL[i][s[i]];
            for (int j = 0; j < 16; ++j) acc[j] ^= row[j];
        }
        for (int i = 0; i < 16; ++i) n[i] = t.piInv[acc[i]] ^ rk[r][i];   // S^-1, затем X[K]
        for (int i = 0; i < 16; ++i) s[i] = n[i];
    }
    for (int i = 0; i < 16; ++i) out[i] = s[i];
}
