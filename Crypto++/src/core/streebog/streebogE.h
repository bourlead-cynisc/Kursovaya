#pragma once
#include <cstdint>
#include "streebogLPS.h"

// E(K, m) = X[K13] LPSX[K12] ... LPSX[K1](m), K[1] = K, K[i] = LPS(K[i-1] xor C[i-1]).
inline void streebogE(const uint8_t K[64], const uint8_t m[64], uint8_t out[64]) {
    const StreebogTables& t = streebogTables();
    uint8_t k[64], s[64];
    for (int i = 0; i < 64; ++i) { k[i] = K[i]; s[i] = m[i]; }
    for (int round = 0; round < 12; ++round) {
        for (int i = 0; i < 64; ++i) s[i] ^= k[i];
        streebogLPS(s, s);
        for (int i = 0; i < 64; ++i) k[i] ^= t.c[round][i];
        streebogLPS(k, k);                                   // следующий раундовый ключ
    }
    for (int i = 0; i < 64; ++i) out[i] = s[i] ^ k[i];       // X[K13]
}
