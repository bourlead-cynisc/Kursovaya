#pragma once
#include <filesystem>
#include <vector>
#include "../Timer.h"
#include "../Log.h"
#include "../formatMs.h"
#include "../logBinary.h"
#include "../generateRandomBytes.h"
#include "../writeRawKeyFile.h"

// Ключ Кузнечика: 256 бит (32 байта). Возвращает ключ, чтобы GUI мог показать его в двоичном виде.
inline std::vector<uint8_t> KuznyechikGenerateKey(const std::filesystem::path& keyFile) {
    Timer timer;
    timer.start();
    std::vector<uint8_t> key = generateRandomBytes(32);
    writeRawKeyFile(keyFile, key);
    Log("Ключ Кузнечика (256 бит) сгенерирован за " + formatMs(timer.elapsedMs()));
    logBinary("  Ключ", key.data(), key.size());
    return key;
}
