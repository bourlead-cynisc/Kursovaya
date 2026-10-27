#pragma once
#include <QLabel>
#include <QStyle>

// Цветная плашка результата. state: "ok" или "bad".
inline void showBanner(QLabel* banner, const QString& text, const char* state) {
    banner->setText(text);
    banner->setProperty("state", state);
    banner->style()->unpolish(banner);
    banner->style()->polish(banner);
    banner->show();
}
