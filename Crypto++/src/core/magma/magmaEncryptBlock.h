#pragma once
#include <cstdint>
#include "../loadBE32.h"
#include "../storeBE32.h"
#include "magmaG.h"

// Шифрование одного 64-битного блока (RFC 8891, п. 5.1).
inline void magmaEncryptBlock(const uint8_t in[8], uint8_t out[8], const uint32_t rk[32]) {
    uint32_t a1 = loadBE32(in), a0 = loadBE32(in + 4);
    for (int i = 0; i < 31; ++i) {            // G[K_i](a1, a0) = (a0, g[K_i](a0) xor a1)
        const uint32_t t = a0;
        a0 = magmaG(rk[i], a0) ^ a1;
        a1 = t;
    }
    storeBE32(out, magmaG(rk[31], a0) ^ a1);  // G*[K_32]
    storeBE32(out + 4, a0);
}
