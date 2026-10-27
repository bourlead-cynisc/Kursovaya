#pragma once
#include <filesystem>
#include <system_error>

inline bool fileExists(const std::filesystem::path& file) {
    std::error_code ec;
    return std::filesystem::is_regular_file(file, ec);
}
