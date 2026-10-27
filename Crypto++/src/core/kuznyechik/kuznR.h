#pragma once
#include <cstdint>
#include "kuznLinear.h"

// R(a_15||...||a_0) = l(a_15,...,a_0)||a_15||...||a_1  (на месте)
inline void kuznR(uint8_t a[16]) {
    const uint8_t t = kuznLinear(a);
    for (int i = 15; i > 0; --i) a[i] = a[i - 1];
    a[0] = t;
}
