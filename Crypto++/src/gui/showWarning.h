#pragma once
#include <QMessageBox>

inline void showWarning(QWidget* parent, const QString& text) {
    QMessageBox::warning(parent, QStringLiteral("Внимание"), text);
}
