#pragma once
#include <cstdint>
#include "kuznLinear.h"

// R^-1(a_15||...||a_0) = a_14||...||a_0||l(a_14,...,a_0,a_15)  (на месте)
inline void kuznRInv(uint8_t a[16]) {
    uint8_t shifted[16];
    for (int i = 0; i < 15; ++i) shifted[i] = a[i + 1];
    shifted[15] = a[0];
    const uint8_t t = kuznLinear(shifted);
    for (int i = 0; i < 15; ++i) a[i] = a[i + 1];
    a[15] = t;
}
