#pragma once
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <vector>

inline void writeRawKeyFile(const std::filesystem::path& keyFile, const std::vector<uint8_t>& key) {
    std::ofstream f(keyFile, std::ios::binary);
    if (!f) throw std::runtime_error("Не удалось создать файл ключа");
    f.write(reinterpret_cast<const char*>(key.data()), static_cast<std::streamsize>(key.size()));
    f.close();
    if (!f) throw std::runtime_error("Ошибка записи файла ключа");
}
