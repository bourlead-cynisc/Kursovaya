#pragma once
#include <cstdint>
#include "MagmaSBox.h"

// t(a) = Pi_7(a_7)||...||Pi_0(a_0): подстановка по 4-битным частям.
inline uint32_t magmaT(uint32_t a) {
    uint32_t r = 0;
    for (int i = 0; i < 8; ++i) {
        const uint32_t nibble = (a >> (4 * i)) & 0xF;
        r |= uint32_t(kMagmaSBox[i][nibble]) << (4 * i);
    }
    return r;
}
