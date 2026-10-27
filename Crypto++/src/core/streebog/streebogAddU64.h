#pragma once
#include <cstdint>

// a := (a + v) mod 2^512, a — little-endian.
inline void streebogAddU64(uint8_t a[64], uint64_t v) {
    unsigned carry = 0;
    for (int i = 0; i < 64; ++i) {
        const unsigned s = unsigned(a[i]) + unsigned(v & 0xFF) + carry;
        a[i] = uint8_t(s);
        carry = s >> 8;
        v >>= 8;
    }
}
