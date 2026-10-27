#pragma once
#include <filesystem>
#include <system_error>

// Удаляет файл, не выбрасывая исключений (чистка недописанного результата).
inline void removeFileQuietly(const std::filesystem::path& file) {
    std::error_code ec;
    std::filesystem::remove(file, ec);
}
