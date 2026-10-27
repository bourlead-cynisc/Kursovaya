#pragma once
#include <cstdint>

// Коэффициенты линейного преобразования l при a_15, a_14, ..., a_0 (RFC 7801, п. 4.2;
// в тексте RFC опечатка "a_15" вместо "a_14" исправлена по errata EID 4660).
inline constexpr uint8_t kKuznyechikLCoef[16] = {
    148, 32, 133, 16, 194, 192, 1, 251, 1, 192, 194, 16, 133, 32, 148, 1
};
