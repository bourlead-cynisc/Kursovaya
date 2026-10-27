#pragma once
#include <QApplication>
#include <QButtonGroup>
#include <QFrame>
#include <QLabel>
#include <QMainWindow>
#include <QStackedWidget>
#include <QVBoxLayout>
#include "AppContext.h"
#include "applyTheme.h"
#include "createButton.h"
#include "../core/Log.h"

// Боковая навигация вместо ряда кнопок AES / RSA / SHA-256 / ECDSA.
inline QWidget* createSidebar(AppContext* ctx, QStackedWidget* stack) {
    auto* frame = new QFrame;
    frame->setObjectName("sidebar");
    frame->setFixedWidth(240);

    auto* v = new QVBoxLayout(frame);
    v->setContentsMargins(16, 22, 16, 16);
    v->setSpacing(6);

    auto* title = new QLabel(QStringLiteral("Crypto++"));
    title->setObjectName("appTitle");
    auto* sub = new QLabel(QStringLiteral("Криптографическая утилита"));
    sub->setObjectName("muted");
    v->addWidget(title);
    v->addWidget(sub);
    v->addSpacing(22);

    auto* section = new QLabel(QStringLiteral("ИНСТРУМЕНТЫ"));
    section->setObjectName("sectionLabel");
    v->addWidget(section);
    v->addSpacing(2);

    struct Item { const char* text; const char* tip; const char* log; };
    const Item items[] = {
        { "AES",     "Симметричное шифрование файлов",  "Выбран режим AES" },
        { "RSA",     "Гибридное шифрование (RSA + AES)", "Выбран режим RSA" },
        { "SHA-256", "Контрольная сумма файла",          "Выбран режим SHA-256" },
        { "ECDSA",   "Цифровая подпись файла",           "Выбран режим ECDSA" },
        { "Магма",    "ГОСТ Р 34.12-2015, блок 64 бита",   "Выбран режим Магма" },
        { "Стрибог",  "ГОСТ Р 34.11-2012, хеш-функция",    "Выбран режим Стрибог" },
        { "Кузнечик", "ГОСТ Р 34.12-2015, блок 128 бит",   "Выбран режим Кузнечик" },
    };

    auto* group = new QButtonGroup(frame);
    group->setExclusive(true);
    for (int i = 0; i < 7; ++i) {
        auto* b = createButton(QString::fromUtf8(items[i].text), ButtonKind::Ghost);
        b->setProperty("variant", QVariant());          // стиль задаёт nav="true"
        b->setProperty("nav", true);
        b->setCheckable(true);
        b->setChecked(i == 0);
        group->addButton(b, i);
        v->addWidget(b);
        b->setToolTip(QString::fromUtf8(items[i].tip));
        const QString msg = QString::fromUtf8(items[i].log);
        QObject::connect(b, &QPushButton::clicked, frame, [=] {
            stack->setCurrentIndex(i);
            Log(msg.toStdString());
        });
    }

    v->addStretch(1);

    auto* theme = createButton(QStringLiteral("☀  Светлая тема"), ButtonKind::Ghost);
    QObject::connect(theme, &QPushButton::clicked, frame, [=] {
        ctx->darkTheme = !ctx->darkTheme;
        applyTheme(ctx->darkTheme);
        theme->setText(ctx->darkTheme ? QStringLiteral("☀  Светлая тема")
                                      : QStringLiteral("☾  Тёмная тема"));
    });
    auto* exit = createButton(QStringLiteral("Выход"), ButtonKind::Ghost);
    QObject::connect(exit, &QPushButton::clicked, ctx->window, &QMainWindow::close);
    v->addWidget(theme);
    v->addWidget(exit);
    return frame;
}
