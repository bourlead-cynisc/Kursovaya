#pragma once
#include <cstdint>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <vector>
#include "BlockFn.h"

// Обратная операция к cbcEncryptGeneric. totalBytes — длина шифртекста (кратна bs, >= bs).
// Бросает std::runtime_error, если дополнение некорректно (неверный ключ / повреждённый файл).
inline void cbcDecryptGeneric(std::istream& in, std::ostream& out, size_t bs, uint64_t totalBytes,
                              const uint8_t* iv, const BlockFn& decryptBlock) {
    std::vector<uint8_t> prev(iv, iv + bs), dec(bs), plain(bs);
    std::vector<uint8_t> buf(bs * 4096);
    uint64_t blocksLeft = totalBytes / bs;
    while (blocksLeft > 0) {
        const size_t want = static_cast<size_t>(std::min<uint64_t>(blocksLeft, 4096)) * bs;
        in.read(reinterpret_cast<char*>(buf.data()), static_cast<std::streamsize>(want));
        if (static_cast<size_t>(in.gcount()) != want) throw std::runtime_error("Файл прочитан не полностью");
        for (size_t b = 0; b < want / bs; ++b, --blocksLeft) {
            const uint8_t* c = &buf[b * bs];
            decryptBlock(c, dec.data());
            for (size_t i = 0; i < bs; ++i) plain[i] = dec[i] ^ prev[i];
            prev.assign(c, c + bs);
            if (blocksLeft == 1) {                    // последний блок: снимаем дополнение
                const uint8_t pad = plain[bs - 1];
                if (pad == 0 || pad > bs) throw std::runtime_error("bad padding");
                for (size_t i = bs - pad; i < bs; ++i)
                    if (plain[i] != pad) throw std::runtime_error("bad padding");
                out.write(reinterpret_cast<const char*>(plain.data()), static_cast<std::streamsize>(bs - pad));
            } else {
                out.write(reinterpret_cast<const char*>(plain.data()), static_cast<std::streamsize>(bs));
            }
        }
    }
}
