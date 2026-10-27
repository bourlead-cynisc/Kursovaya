#pragma once
#include <QLabel>
#include <QVBoxLayout>

inline QWidget* createPageHeader(const QString& title, const QString& subtitle) {
    auto* w = new QWidget;
    auto* v = new QVBoxLayout(w);
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(4);
    auto* t = new QLabel(title);
    t->setObjectName("pageTitle");
    auto* s = new QLabel(subtitle);
    s->setObjectName("muted");
    s->setWordWrap(true);
    v->addWidget(t);
    v->addWidget(s);
    return w;
}
