#pragma once
#include <filesystem>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include "ECDSAKeyTypes.h"
#include "loadECDSAPublicKey.h"
#include "computeFileSHA256.h"
#include "Timer.h"
#include "Log.h"
#include "formatMs.h"

// Возвращает true, если подпись верна.
inline bool ECDSAVerify(const std::filesystem::path& inputFile,
                        const std::filesystem::path& pubKeyFile,
                        const std::filesystem::path& sigFile) {
    using namespace CryptoPP;
    Timer timer;
    timer.start();

    ECDSAPublicKey publicKey;
    loadECDSAPublicKey(pubKeyFile, publicKey);

    std::ifstream sigF(sigFile, std::ios::binary);
    if (!sigF) throw std::runtime_error("Не удалось открыть файл подписи");
    std::string signature((std::istreambuf_iterator<char>(sigF)), std::istreambuf_iterator<char>());
    sigF.close();

    FileDigest d = computeFileSHA256(inputFile);

    ECDSA<ECP, SHA256>::Verifier verifier(publicKey);
    const bool ok = verifier.VerifyMessage(d.bytes.data(), d.bytes.size(),
        reinterpret_cast<const CryptoPP::byte*>(signature.data()), signature.size());

    Log(std::string(ok ? "Подпись ECDSA верна" : "Подпись ECDSA НЕВЕРНА") +
        "! Проверка заняла " + formatMs(timer.elapsedMs()));
    return ok;
}
