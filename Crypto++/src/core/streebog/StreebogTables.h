#pragma once
#include <cstdint>

struct StreebogTables {
    uint8_t  c[12][64];       // константы C[1..12] в little-endian
    uint64_t t2[8][256];      // t2[pos][b] = l( Pi[b] << (8*pos) ) — подстановка S и преобразование L вместе
};
