#pragma once
#include <filesystem>

// my.key -> my_pub.key (в той же папке)
inline std::filesystem::path ecdsaPublicKeyPathFor(const std::filesystem::path& privFile) {
    std::filesystem::path p = privFile;
    p.replace_extension();
    p += "_pub.key";
    return p;
}
