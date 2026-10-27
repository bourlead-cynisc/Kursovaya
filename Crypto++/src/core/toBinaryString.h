#pragma once
#include <cstddef>
#include <cstdint>
#include <string>

// Данные -> двоичная запись (0/1). Байты разделены пробелом, строки по bytesPerLine байт.
inline std::string toBinaryString(const uint8_t* data, size_t size, size_t bytesPerLine = 8) {
    std::string s;
    s.reserve(size * 9);
    for (size_t i = 0; i < size; ++i) {
        for (int bit = 7; bit >= 0; --bit) s += ((data[i] >> bit) & 1) ? '1' : '0';
        if (i + 1 < size) s += ((i + 1) % bytesPerLine == 0) ? '\n' : ' ';
    }
    return s;
}
