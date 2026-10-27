#pragma once
#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <string>
#include <cryptopp/integer.h>
#include <cryptopp/osrng.h>
#include <cryptopp/rsa.h>
#include "Timer.h"
#include "Log.h"
#include "formatMs.h"
#include "saveRSAPrivateKey.h"
#include "saveRSAPublicKey.h"

// Создаёт private_<N>.key и public_<N>.key в папке keyDir.
inline void RSAGenerateKeys(int keySize, const std::filesystem::path& keyDir) {
    if (!std::filesystem::is_directory(keyDir)) throw std::runtime_error("Папка ключей не существует");

    Timer timer;
    timer.start();

    CryptoPP::AutoSeededRandomPool rng;
    CryptoPP::RSA::PrivateKey privateKey;
    privateKey.GenerateRandomWithKeySize(rng, static_cast<unsigned>(keySize));
    CryptoPP::RSA::PublicKey publicKey(privateKey);

    saveRSAPrivateKey(keyDir / ("private_" + std::to_string(keySize) + ".key"), privateKey);
    saveRSAPublicKey(keyDir / ("public_" + std::to_string(keySize) + ".key"), publicKey);

    std::ostringstream oss;
    oss << privateKey.GetModulus();

    Log("RSA " + std::to_string(keySize) + "-bit ключи сгенерированы за " + formatMs(timer.elapsedMs()));
    Log("  Модуль: " + std::to_string(oss.str().length()) + " цифр");
}
