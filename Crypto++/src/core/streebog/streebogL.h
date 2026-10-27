#pragma once
#include <cstdint>
#include "StreebogA.h"

// Линейное преобразование l над 64-битным вектором (RFC 6986, п. 6.4):
// бит i числа w (i = 0..63) выбирает строку A[63 - i]; результат — XOR выбранных строк.
inline uint64_t streebogL(uint64_t w) {
    uint64_t r = 0;
    for (int i = 0; i < 64; ++i)
        if ((w >> i) & 1) r ^= kStreebogA[63 - i];
    return r;
}
