#pragma once
#include <sstream>
#include <string>
#include "Log.h"
#include "toBinaryString.h"

// Пишет двоичные данные в журнал: заголовок и по строке на каждые bytesPerLine байт.
inline void logBinary(const std::string& title, const uint8_t* data, size_t size, size_t bytesPerLine = 8) {
    Log(title + " (" + std::to_string(size * 8) + " бит):");
    std::istringstream rows(toBinaryString(data, size, bytesPerLine));
    std::string row;
    while (std::getline(rows, row)) Log("  " + row);
}
