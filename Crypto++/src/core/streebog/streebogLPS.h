#pragma once
#include <cstdint>
#include "../loadLE64.h"
#include "../storeLE64.h"
#include "StreebogTau.h"
#include "streebogTables.h"

// LPS(x) = L(P(S(x))) для 512-битного вектора (64 байта, little-endian). in и out могут совпадать.
inline void streebogLPS(const uint8_t in[64], uint8_t out[64]) {
    const StreebogTables& t = streebogTables();
    uint64_t w[8];
    for (int j = 0; j < 8; ++j) {
        uint64_t acc = 0;
        for (int pos = 0; pos < 8; ++pos)
            acc ^= t.t2[pos][in[kStreebogTau[8 * j + pos]]];   // P: out[i] = in[Tau[i]]; затем S и L
        w[j] = acc;
    }
    for (int j = 0; j < 8; ++j) storeLE64(out + 8 * j, w[j]);
}
