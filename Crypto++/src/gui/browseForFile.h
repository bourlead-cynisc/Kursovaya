#pragma once
#include <QDir>
#include <QFileDialog>
#include <QString>

inline QString browseForFile(QWidget* parent, const QString& title,
                             const QString& filter = QStringLiteral("Все файлы (*)")) {
    const QString f = QFileDialog::getOpenFileName(parent, title, QString(), filter);
    return f.isEmpty() ? f : QDir::toNativeSeparators(f);
}
