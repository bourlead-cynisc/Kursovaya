#pragma once
#include <cstdint>
#include "../loadBE32.h"
#include "../storeBE32.h"
#include "magmaG.h"

// Расшифровка одного блока (RFC 8891, п. 5.2): те же раунды с ключами в обратном порядке.
inline void magmaDecryptBlock(const uint8_t in[8], uint8_t out[8], const uint32_t rk[32]) {
    uint32_t a1 = loadBE32(in), a0 = loadBE32(in + 4);
    for (int i = 31; i > 0; --i) {
        const uint32_t t = a0;
        a0 = magmaG(rk[i], a0) ^ a1;
        a1 = t;
    }
    storeBE32(out, magmaG(rk[0], a0) ^ a1);
    storeBE32(out + 4, a0);
}
