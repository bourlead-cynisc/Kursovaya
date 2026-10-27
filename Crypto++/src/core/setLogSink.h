#pragma once
#include "logSinkStorage.h"

inline void setLogSink(LogSink sink) {
    logSinkStorage() = std::move(sink);
}
