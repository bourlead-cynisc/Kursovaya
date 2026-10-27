#pragma once
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPointer>
#include <QProgressBar>
#include <QVBoxLayout>
#include "AppContext.h"
#include "createButton.h"
#include "../core/Log.h"
#include "../core/setLogSink.h"

// Панель журнала. Подключает core::Log к виджету; безопасно для вызовов из фоновых потоков.
inline QWidget* createLogPanel(AppContext* ctx) {
    auto* frame = new QFrame;
    frame->setObjectName("card");
    auto* v = new QVBoxLayout(frame);
    v->setContentsMargins(22, 14, 22, 18);
    v->setSpacing(10);

    auto* head = new QHBoxLayout;
    auto* title = new QLabel(QStringLiteral("Журнал"));
    title->setObjectName("cardTitle");
    auto* status = new QLabel(QStringLiteral("Готово"));
    status->setObjectName("muted");
    auto* clear = createButton(QStringLiteral("Очистить"), ButtonKind::Ghost);
    clear->setMinimumHeight(30);
    head->addWidget(title);
    head->addStretch(1);
    head->addWidget(status);
    head->addWidget(clear);

    auto* view = new QPlainTextEdit;
    view->setObjectName("log");
    view->setReadOnly(true);
    view->setMaximumBlockCount(5000);
    view->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    view->setPlaceholderText(QStringLiteral("Здесь появятся сообщения о ходе операций…"));

    v->addLayout(head);
    v->addWidget(view, 1);

    ctx->logView = view;
    ctx->statusLabel = status;

    QObject::connect(clear, &QPushButton::clicked, view, [view] {
        view->clear();
        Log("Журнал очищен");
    });

    QPointer<QPlainTextEdit> guard(view);
    setLogSink([guard](const std::string& line) {
        const QString text = QString::fromStdString(line);
        QMetaObject::invokeMethod(qApp, [guard, text] {
            if (guard) guard->appendPlainText(text);
        }, Qt::QueuedConnection);
    });
    return frame;
}
