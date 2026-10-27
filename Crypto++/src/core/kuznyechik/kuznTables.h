#pragma once
#include <cstring>
#include "../GostPi.h"
#include "KuznyechikTables.h"
#include "kuznL.h"
#include "kuznLInv.h"

inline const KuznyechikTables& kuznTables() {
    static const KuznyechikTables* tables = [] {
        auto* t = new KuznyechikTables;
        for (int i = 0; i < 256; ++i) { t->pi[i] = kGostPi[i]; t->piInv[kGostPi[i]] = uint8_t(i); }
        for (int pos = 0; pos < 16; ++pos) {
            for (int v = 0; v < 256; ++v) {
                uint8_t a[16] = {0};
                a[pos] = uint8_t(v);
                kuznL(a);
                std::memcpy(t->encL[pos][v], a, 16);
                uint8_t b[16] = {0};
                b[pos] = uint8_t(v);
                kuznLInv(b);
                std::memcpy(t->decL[pos][v], b, 16);
            }
        }
        for (int i = 1; i <= 32; ++i) {
            uint8_t a[16] = {0};
            a[15] = uint8_t(i);        // Vec_128(i)
            kuznL(a);
            std::memcpy(t->c[i - 1], a, 16);
        }
        return t;
    }();
    return *tables;
}
