#pragma once
#include <cstdint>

// Умножение в поле GF(2)[x]/(x^8+x^7+x^6+x+1) (RFC 7801, п. 3.2).
inline uint8_t kuznGfMul(uint8_t a, uint8_t b) {
    uint8_t r = 0;
    while (b) {
        if (b & 1) r ^= a;
        const bool carry = (a & 0x80) != 0;
        a = static_cast<uint8_t>(a << 1);
        if (carry) a ^= 0xC3;      // 0x1C3 без старшего бита
        b >>= 1;
    }
    return r;
}
