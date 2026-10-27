#pragma once
#include <cstdint>
#include "streebogE.h"

// Функция сжатия: h := g_N(h, m) = E(LPS(h xor N), m) xor h xor m.
inline void streebogG(const uint8_t N[64], uint8_t h[64], const uint8_t m[64]) {
    uint8_t k[64], e[64];
    for (int i = 0; i < 64; ++i) k[i] = h[i] ^ N[i];
    streebogLPS(k, k);
    streebogE(k, m, e);
    for (int i = 0; i < 64; ++i) h[i] = e[i] ^ h[i] ^ m[i];
}
