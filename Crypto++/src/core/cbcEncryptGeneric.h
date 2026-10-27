#pragma once
#include <cstdint>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <vector>
#include "BlockFn.h"

// CBC + дополнение PKCS#7 для блочного шифра с произвольным размером блока (8 или 16 байт).
inline void cbcEncryptGeneric(std::istream& in, std::ostream& out, size_t bs,
                              const uint8_t* iv, const BlockFn& encryptBlock) {
    std::vector<uint8_t> prev(iv, iv + bs), tmp(bs), enc(bs);
    std::vector<uint8_t> buf(bs * 4096);   // кратно размеру блока
    while (true) {
        in.read(reinterpret_cast<char*>(buf.data()), static_cast<std::streamsize>(buf.size()));
        const size_t got = static_cast<size_t>(in.gcount());
        if (in.bad()) throw std::runtime_error("Ошибка чтения входного файла");
        const size_t full = got / bs;
        for (size_t b = 0; b < full; ++b) {
            for (size_t i = 0; i < bs; ++i) tmp[i] = buf[b * bs + i] ^ prev[i];
            encryptBlock(tmp.data(), enc.data());
            out.write(reinterpret_cast<const char*>(enc.data()), static_cast<std::streamsize>(bs));
            prev = enc;
        }
        if (got < buf.size()) {                       // конец файла: последний блок с дополнением
            const size_t rem = got - full * bs;
            const uint8_t pad = static_cast<uint8_t>(bs - rem);
            for (size_t i = 0; i < bs; ++i)
                tmp[i] = (i < rem ? buf[full * bs + i] : pad) ^ prev[i];
            encryptBlock(tmp.data(), enc.data());
            out.write(reinterpret_cast<const char*>(enc.data()), static_cast<std::streamsize>(bs));
            return;
        }
    }
}
