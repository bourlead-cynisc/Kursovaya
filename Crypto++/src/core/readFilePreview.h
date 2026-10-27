#pragma once
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <vector>

// Читает до count байт файла, начиная со смещения offset (для показа примера данных).
inline std::vector<uint8_t> readFilePreview(const std::filesystem::path& file, uint64_t offset, size_t count) {
    std::ifstream f(file, std::ios::binary);
    std::vector<uint8_t> buf(count);
    if (!f) return {};
    f.seekg(static_cast<std::streamoff>(offset));
    f.read(reinterpret_cast<char*>(buf.data()), static_cast<std::streamsize>(count));
    buf.resize(static_cast<size_t>(f.gcount()));
    return buf;
}
