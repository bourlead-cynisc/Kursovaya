#pragma once
#include <algorithm>
#include <cstring>
#include "../GostPi.h"
#include "../hexToBytes.h"
#include "StreebogC.h"
#include "StreebogTables.h"
#include "streebogL.h"

inline const StreebogTables& streebogTables() {
    static const StreebogTables* tables = [] {
        auto* t = new StreebogTables;
        for (int i = 0; i < 12; ++i) {
            auto bytes = hexToBytes(kStreebogCHex[i]);          // старший байт первым
            std::reverse(bytes.begin(), bytes.end());           // -> little-endian
            std::memcpy(t->c[i], bytes.data(), 64);
        }
        for (int pos = 0; pos < 8; ++pos)
            for (int b = 0; b < 256; ++b)
                t->t2[pos][b] = streebogL(uint64_t(kGostPi[b]) << (8 * pos));
        return t;
    }();
    return *tables;
}
