#pragma once
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

// Читает ключ из файла и проверяет его длину в байтах.
inline std::vector<uint8_t> readRawKeyFile(const std::filesystem::path& keyFile, size_t expectedBytes) {
    std::ifstream f(keyFile, std::ios::binary);
    if (!f) throw std::runtime_error("Не удалось открыть файл ключа");
    std::vector<uint8_t> key((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    if (key.size() != expectedBytes)
        throw std::runtime_error("Неверный размер ключа: нужно " + std::to_string(expectedBytes) +
                                 " байт, в файле " + std::to_string(key.size()));
    return key;
}
