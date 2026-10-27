#pragma once
#include <functional>
#include <string>

using LogSink = std::function<void(const std::string&)>;

// Хранилище приёмника журнала (заменяет глобальный HWND hEditLog).
inline LogSink& logSinkStorage() {
    static LogSink sink;
    return sink;
}
