#pragma once
#include <cstring>
#include <stdexcept>
#include "StreebogContext.h"

inline void streebogInit(StreebogContext& ctx, int bits) {
    if (bits != 256 && bits != 512) throw std::invalid_argument("Стрибог: длина хеша 256 или 512 бит");
    std::memset(&ctx, 0, sizeof(ctx));
    ctx.bits = bits;
    if (bits == 256) std::memset(ctx.h, 0x01, 64);   // IV = (00000001)^64; для 512 бит IV = 0
}
