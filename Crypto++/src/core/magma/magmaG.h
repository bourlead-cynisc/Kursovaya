#pragma once
#include <cstdint>
#include "magmaT.h"

// g[k](a) = t(a + k mod 2^32) <<< 11
inline uint32_t magmaG(uint32_t k, uint32_t a) {
    const uint32_t t = magmaT(a + k);
    return (t << 11) | (t >> 21);
}
