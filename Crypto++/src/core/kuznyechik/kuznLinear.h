#pragma once
#include <cstdint>
#include "KuznyechikLCoef.h"
#include "kuznGfMul.h"

// l(a_15,...,a_0): a[0] = a_15 ... a[15] = a_0 (порядок байт как в записи строки слева направо).
inline uint8_t kuznLinear(const uint8_t a[16]) {
    uint8_t r = 0;
    for (int i = 0; i < 16; ++i) r ^= kuznGfMul(a[i], kKuznyechikLCoef[i]);
    return r;
}
