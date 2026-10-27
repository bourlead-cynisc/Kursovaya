#pragma once
#include <filesystem>
#include <QString>

// QString -> std::filesystem::path (на Windows через wchar_t, чтобы работали кириллические пути)
inline std::filesystem::path toPath(const QString& s) {
#ifdef _WIN32
    return std::filesystem::path(s.toStdWString());
#else
    return std::filesystem::path(s.toStdString());
#endif
}
