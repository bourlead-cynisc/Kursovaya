#pragma once
#include <filesystem>
#include <string>

// Путь -> UTF-8 строка (работает и для C++17, и для C++20).
inline std::string pathToUtf8(const std::filesystem::path& p) {
    auto s = p.u8string();
    return std::string(s.begin(), s.end());
}
