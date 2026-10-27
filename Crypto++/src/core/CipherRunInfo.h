#pragma once
#include <cstdint>
#include <filesystem>
#include <vector>

// Что вернула операция Магмы/Кузнечика: путь результата и данные для показа в двоичном виде.
struct CipherRunInfo {
    std::filesystem::path outputFile;
    std::vector<uint8_t> iv;        // вектор инициализации (при шифровании)
    std::vector<uint8_t> preview;   // первые байты шифртекста / открытого текста
    uint64_t inputSize = 0;
    uint64_t outputSize = 0;
};
