#pragma once
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <vector>
#include "../FileHash.h"
#include "streebogInit.h"
#include "streebogUpdate.h"
#include "streebogFinal.h"

// Хеш файла (потоково, блоками по 1 МБ).
inline FileHash computeFileStreebog(const std::filesystem::path& file, int bits) {
    std::ifstream in(file, std::ios::binary);
    if (!in) throw std::runtime_error("Не удалось открыть входной файл");

    StreebogContext ctx;
    streebogInit(ctx, bits);
    std::vector<uint8_t> buf(1 << 20);
    uint64_t total = 0;
    while (true) {
        in.read(reinterpret_cast<char*>(buf.data()), static_cast<std::streamsize>(buf.size()));
        const size_t got = static_cast<size_t>(in.gcount());
        if (in.bad()) throw std::runtime_error("Ошибка чтения файла");
        streebogUpdate(ctx, buf.data(), got);
        total += got;
        if (got < buf.size()) break;
    }
    FileHash r;
    r.bytes.resize(static_cast<size_t>(bits / 8));
    streebogFinal(ctx, r.bytes.data());
    r.size = total;
    return r;
}
