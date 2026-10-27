#pragma once
#include <QPushButton>
#include "ButtonKind.h"

inline QPushButton* createButton(const QString& text, ButtonKind kind = ButtonKind::Secondary,
                                 QWidget* parent = nullptr) {
    auto* b = new QPushButton(text, parent);
    b->setCursor(Qt::PointingHandCursor);
    b->setMinimumHeight(38);
    switch (kind) {
    case ButtonKind::Primary: b->setProperty("variant", "primary"); break;
    case ButtonKind::Ghost:   b->setProperty("variant", "ghost");   break;
    default: break;
    }
    return b;
}
