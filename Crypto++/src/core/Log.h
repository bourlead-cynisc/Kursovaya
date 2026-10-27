#pragma once
#include <string>
#include "getCurrentTime.h"
#include "logSinkStorage.h"

// Пишет строку в журнал с меткой времени. Куда именно -- решает GUI (setLogSink).
inline void Log(const std::string& text) {
    const LogSink& sink = logSinkStorage();
    if (sink) sink("[" + getCurrentTime() + "] " + text);
}
