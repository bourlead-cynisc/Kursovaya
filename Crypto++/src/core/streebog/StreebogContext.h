#pragma once
#include <cstdint>

// Состояние хеширования. Все 512-битные значения хранятся в little-endian
// (байт 0 — младший), так что байты сообщения ложатся в память «как есть».
struct StreebogContext {
    uint8_t h[64];        // текущий хеш
    uint8_t N[64];        // счётчик обработанных бит
    uint8_t sigma[64];    // контрольная сумма блоков (EPSILON в RFC)
    uint8_t buf[64];      // неполный блок
    size_t  bufLen;
    int     bits;         // 256 или 512
};
