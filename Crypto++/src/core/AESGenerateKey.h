#pragma once
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <cryptopp/osrng.h>
#include <cryptopp/secblock.h>
#include "Timer.h"
#include "Log.h"
#include "formatMs.h"

inline void AESGenerateKey(int keySizeBits, const std::filesystem::path& keyFile) {
    if (keySizeBits != 128 && keySizeBits != 192 && keySizeBits != 256)
        throw std::runtime_error("Допустимые размеры ключа AES: 128, 192, 256 бит");

    Timer timer;
    timer.start();

    CryptoPP::AutoSeededRandomPool rng;
    CryptoPP::SecByteBlock key(static_cast<size_t>(keySizeBits / 8));
    rng.GenerateBlock(key, key.size());

    std::ofstream file(keyFile, std::ios::binary);
    if (!file) throw std::runtime_error("Не удалось создать файл ключа");
    file.write(reinterpret_cast<const char*>(key.data()), static_cast<std::streamsize>(key.size()));
    file.close();

    Log("Ключ AES-" + std::to_string(keySizeBits) + " сгенерирован за " + formatMs(timer.elapsedMs()));
}
