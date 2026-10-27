#pragma once
#include <istream>
#include <ostream>
#include <cryptopp/aes.h>
#include <cryptopp/files.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/secblock.h>

// AES-CBC (PKCS#7) из потока в поток. Паддинг добавляется один раз, в конце.
inline void cbcEncryptStream(std::istream& in, std::ostream& out,
                             const CryptoPP::SecByteBlock& key, const CryptoPP::byte* iv) {
    CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption enc;
    enc.SetKeyWithIV(key, key.size(), iv);
    CryptoPP::FileSource src(in, true,
        new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::FileSink(out)));
}
