#pragma once
#include <QColor>
#include <QHash>
#include <QPalette>

// Палитра для стандартных диалогов и виджетов, которые не покрывает QSS.
inline QPalette buildPalette(const QHash<QString, QString>& t) {
    QPalette p;
    p.setColor(QPalette::Window,          QColor(t["bg"]));
    p.setColor(QPalette::WindowText,      QColor(t["text"]));
    p.setColor(QPalette::Base,            QColor(t["surface2"]));
    p.setColor(QPalette::AlternateBase,   QColor(t["surface"]));
    p.setColor(QPalette::Text,            QColor(t["text"]));
    p.setColor(QPalette::Button,          QColor(t["surface2"]));
    p.setColor(QPalette::ButtonText,      QColor(t["text"]));
    p.setColor(QPalette::ToolTipBase,     QColor(t["surface"]));
    p.setColor(QPalette::ToolTipText,     QColor(t["text"]));
    p.setColor(QPalette::PlaceholderText, QColor(t["muted"]));
    p.setColor(QPalette::Highlight,       QColor(t["accent"]));
    p.setColor(QPalette::HighlightedText, QColor(t["onAccent"]));
    return p;
}
