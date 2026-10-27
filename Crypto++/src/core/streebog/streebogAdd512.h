#pragma once
#include <cstdint>

// a := (a + b) mod 2^512, оба — little-endian.
inline void streebogAdd512(uint8_t a[64], const uint8_t b[64]) {
    unsigned carry = 0;
    for (int i = 0; i < 64; ++i) {
        const unsigned s = unsigned(a[i]) + unsigned(b[i]) + carry;
        a[i] = uint8_t(s);
        carry = s >> 8;
    }
}
