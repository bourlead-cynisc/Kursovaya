#pragma once
#include <cstdint>
#include "kuznR.h"

// L = R^16
inline void kuznL(uint8_t a[16]) {
    for (int i = 0; i < 16; ++i) kuznR(a);
}
