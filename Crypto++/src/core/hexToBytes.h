#pragma once
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

// "00ff.." -> байты (пробелы и переводы строк игнорируются). Используется для таблиц констант.
inline std::vector<uint8_t> hexToBytes(const std::string& hex) {
    std::vector<uint8_t> out;
    int hi = -1;
    for (char c : hex) {
        int v;
        if (c >= '0' && c <= '9') v = c - '0';
        else if (c >= 'a' && c <= 'f') v = c - 'a' + 10;
        else if (c >= 'A' && c <= 'F') v = c - 'A' + 10;
        else if (c == ' ' || c == '\n' || c == '\r' || c == '\t') continue;
        else throw std::invalid_argument("hexToBytes: недопустимый символ");
        if (hi < 0) hi = v; else { out.push_back(uint8_t(hi * 16 + v)); hi = -1; }
    }
    if (hi >= 0) throw std::invalid_argument("hexToBytes: нечётное число символов");
    return out;
}
