#pragma once
#include <cstring>
#include "StreebogContext.h"
#include "streebogG.h"
#include "streebogAdd512.h"
#include "streebogAddU64.h"

// Полные блоки обрабатываются сразу: по стандарту после них всегда идёт добивающий блок.
inline void streebogUpdate(StreebogContext& ctx, const uint8_t* data, size_t len) {
    while (len > 0) {
        const size_t take = (64 - ctx.bufLen < len) ? 64 - ctx.bufLen : len;
        std::memcpy(ctx.buf + ctx.bufLen, data, take);
        ctx.bufLen += take;
        data += take;
        len -= take;
        if (ctx.bufLen == 64) {
            streebogG(ctx.N, ctx.h, ctx.buf);
            streebogAddU64(ctx.N, 512);
            streebogAdd512(ctx.sigma, ctx.buf);
            ctx.bufLen = 0;
        }
    }
}
