#pragma once
#include <cstdint>
#include "kuznRInv.h"

// L^-1 = (R^-1)^16
inline void kuznLInv(uint8_t a[16]) {
    for (int i = 0; i < 16; ++i) kuznRInv(a);
}
