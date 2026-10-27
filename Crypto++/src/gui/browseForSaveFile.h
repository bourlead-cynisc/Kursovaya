#pragma once
#include <QDir>
#include <QFileDialog>
#include <QString>

inline QString browseForSaveFile(QWidget* parent, const QString& title, const QString& filter,
                                 const QString& suggestedName = QString()) {
    const QString f = QFileDialog::getSaveFileName(parent, title, suggestedName, filter);
    return f.isEmpty() ? f : QDir::toNativeSeparators(f);
}
