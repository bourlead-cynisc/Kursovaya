#pragma once
#include <filesystem>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <cryptopp/secblock.h>

// Читает AES-ключ из файла и проверяет, что длина совпадает с выбранной.
inline CryptoPP::SecByteBlock readAESKeyFile(const std::filesystem::path& keyFile, int keySizeBits) {
    std::ifstream f(keyFile, std::ios::binary);
    if (!f) throw std::runtime_error("Не удалось открыть файл ключа");
    std::string s((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

    const size_t expected = static_cast<size_t>(keySizeBits / 8);
    if (s.size() != expected) {
        throw std::runtime_error("Неверный размер ключа: нужно " + std::to_string(expected) +
                                 " байт, в файле " + std::to_string(s.size()));
    }
    return CryptoPP::SecByteBlock(reinterpret_cast<const CryptoPP::byte*>(s.data()), s.size());
}
