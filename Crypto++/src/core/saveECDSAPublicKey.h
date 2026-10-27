#pragma once
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <cryptopp/files.h>
#include "ECDSAKeyTypes.h"
inline void saveECDSAPublicKey(const std::filesystem::path& file, const ECDSAPublicKey& key) {
    std::ofstream out(file, std::ios::binary);
    if (!out) throw std::runtime_error("Не удалось создать файл ключа");
    CryptoPP::FileSink sink(out);
    key.Save(sink);
    out.flush();
    if (!out) throw std::runtime_error("Ошибка записи файла ключа");
}
