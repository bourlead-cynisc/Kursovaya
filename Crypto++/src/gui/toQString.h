#pragma once
#include <filesystem>
#include <QDir>
#include <QString>

inline QString toQString(const std::filesystem::path& p) {
#ifdef _WIN32
    return QDir::toNativeSeparators(QString::fromStdWString(p.wstring()));
#else
    return QString::fromStdString(p.string());
#endif
}
