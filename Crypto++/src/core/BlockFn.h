#pragma once
#include <cstdint>
#include <functional>

// Шифрование/расшифровка одного блока: out = f(in).
using BlockFn = std::function<void(const uint8_t* in, uint8_t* out)>;
