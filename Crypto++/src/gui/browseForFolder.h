#pragma once
#include <QDir>
#include <QFileDialog>
#include <QString>

inline QString browseForFolder(QWidget* parent, const QString& title) {
    const QString d = QFileDialog::getExistingDirectory(parent, title);
    return d.isEmpty() ? d : QDir::toNativeSeparators(d);
}
