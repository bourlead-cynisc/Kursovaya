#pragma once
#include <iomanip>
#include <sstream>
#include <string>

// Форматирует миллисекунды с двумя знаками после запятой.
inline std::string formatMs(double ms) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << ms << " ms";
    return oss.str();
}
