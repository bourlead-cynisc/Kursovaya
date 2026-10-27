#pragma once
#include <string>
#include "toBinaryString.h"

// Блок для вывода: заголовок с длиной в битах + данные в двоичном виде.
inline std::string toBinarySection(const std::string& title, const uint8_t* data, size_t size,
                                   size_t bytesPerLine = 8) {
    return title + " (" + std::to_string(size * 8) + " бит):\n" +
           toBinaryString(data, size, bytesPerLine) + "\n";
}
