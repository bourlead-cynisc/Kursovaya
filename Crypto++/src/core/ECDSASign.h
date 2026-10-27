#pragma once
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <cryptopp/filters.h>
#include <cryptopp/osrng.h>
#include "ECDSAKeyTypes.h"
#include "loadECDSAPrivateKey.h"
#include "computeFileSHA256.h"
#include "Timer.h"
#include "Log.h"
#include "formatMs.h"
#include "formatFileSize.h"

// Подписывает SHA-256 файла закрытым ключом ECDSA, подпись пишет в sigFile.
inline void ECDSASign(const std::filesystem::path& inputFile,
                      const std::filesystem::path& privKeyFile,
                      const std::filesystem::path& sigFile) {
    using namespace CryptoPP;
    Timer timer;
    timer.start();

    ECDSAPrivateKey privateKey;
    loadECDSAPrivateKey(privKeyFile, privateKey);

    FileDigest d = computeFileSHA256(inputFile);

    AutoSeededRandomPool rng;
    ECDSA<ECP, SHA256>::Signer signer(privateKey);
    std::string signature;
    StringSource(d.bytes.data(), d.bytes.size(), true,
        new SignerFilter(rng, signer, new StringSink(signature)));

    std::ofstream sigF(sigFile, std::ios::binary);
    if (!sigF) throw std::runtime_error("Не удалось создать файл подписи");
    sigF.write(signature.data(), static_cast<std::streamsize>(signature.size()));
    sigF.close();

    Log("Подпись ECDSA создана за " + formatMs(timer.elapsedMs()));
    Log("  Размер файла: " + formatFileSize(d.size));
    Log("  Размер подписи: " + std::to_string(signature.size()) + " байт");
}
