#pragma once
#include <filesystem>
#include <stdexcept>
#include <string>
#include <cryptopp/osrng.h>
#include "ECDSAKeyTypes.h"
#include "saveECDSAPrivateKey.h"
#include "saveECDSAPublicKey.h"
#include "Timer.h"
#include "Log.h"
#include "formatMs.h"

inline void ECDSAGenerateKeys(const std::string& curve,
                              const std::filesystem::path& privFile,
                              const std::filesystem::path& pubFile) {
    using namespace CryptoPP;
    Timer timer;
    timer.start();

    AutoSeededRandomPool rng;
    ECDSAPrivateKey privateKey;

    if (curve == "P-256") privateKey.Initialize(rng, ASN1::secp256r1());
    else if (curve == "P-384") privateKey.Initialize(rng, ASN1::secp384r1());
    else if (curve == "P-521") privateKey.Initialize(rng, ASN1::secp521r1());
    else throw std::runtime_error("Неизвестная кривая");

    ECDSAPublicKey publicKey;
    privateKey.MakePublicKey(publicKey);

    saveECDSAPrivateKey(privFile, privateKey);
    saveECDSAPublicKey(pubFile, publicKey);

    Log("Ключи ECDSA (" + curve + ") сгенерированы за " + formatMs(timer.elapsedMs()));
}
