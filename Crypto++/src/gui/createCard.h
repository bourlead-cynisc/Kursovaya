#pragma once
#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include "Card.h"

// Карточка с заголовком и (необязательным) пояснением.
inline Card createCard(const QString& title, const QString& subtitle = QString(),
                       QWidget* parent = nullptr) {
    auto* frame = new QFrame(parent);
    frame->setObjectName("card");
    auto* body = new QVBoxLayout(frame);
    body->setContentsMargins(22, 18, 22, 22);
    body->setSpacing(14);

    auto* t = new QLabel(title);
    t->setObjectName("cardTitle");
    body->addWidget(t);
    if (!subtitle.isEmpty()) {
        auto* s = new QLabel(subtitle);
        s->setObjectName("muted");
        s->setWordWrap(true);
        body->addWidget(s);
    }
    return { frame, body };
}
