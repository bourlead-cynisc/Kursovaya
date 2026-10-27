#pragma once
#include <cstring>
#include "StreebogContext.h"
#include "streebogG.h"
#include "streebogAdd512.h"
#include "streebogAddU64.h"

// Завершает хеширование. out получает bits/8 байт (256 бит — старшие 32 байта h) в порядке памяти.
inline void streebogFinal(StreebogContext& ctx, uint8_t* out) {
    uint8_t m[64];
    std::memset(m, 0, 64);
    std::memcpy(m, ctx.buf, ctx.bufLen);
    m[ctx.bufLen] = 0x01;                                   // дополнение: 0^(511-|M|) || 1 || M

    streebogG(ctx.N, ctx.h, m);
    streebogAddU64(ctx.N, static_cast<uint64_t>(ctx.bufLen) * 8);
    streebogAdd512(ctx.sigma, m);

    uint8_t zero[64];
    std::memset(zero, 0, 64);
    streebogG(zero, ctx.h, ctx.N);
    streebogG(zero, ctx.h, ctx.sigma);

    if (ctx.bits == 512) std::memcpy(out, ctx.h, 64);
    else                 std::memcpy(out, ctx.h + 32, 32);  // MSB_256
}
