#pragma once
#include <exception>
#include <functional>
#include <QFutureWatcher>
#include <QMessageBox>
#include <QtConcurrent>
#include "AppContext.h"
#include "setBusy.h"
#include "../core/Log.h"

// Выполняет work() в фоновом потоке (GUI не зависает, например при генерации RSA-16384),
// затем в GUI-потоке вызывает onSuccess() или показывает ошибку.
inline void runTask(AppContext* ctx, const QString& title,
                    std::function<void()> work, std::function<void()> onSuccess = nullptr) {
    if (ctx->busy) return;
    setBusy(ctx, true, title + QStringLiteral("…"));

    auto* watcher = new QFutureWatcher<QString>(ctx);
    QObject::connect(watcher, &QFutureWatcher<QString>::finished, ctx, [=] {
        const QString err = watcher->result();
        watcher->deleteLater();
        setBusy(ctx, false);
        if (!err.isEmpty()) {
            Log("Ошибка: " + err.toStdString());
            QMessageBox::critical(ctx->window, title, err);
        } else if (onSuccess) {
            onSuccess();
        }
    });
    watcher->setFuture(QtConcurrent::run([work]() -> QString {
        try {
            work();
            return QString();
        } catch (const std::exception& e) {
            return QString::fromUtf8(e.what());
        } catch (...) {
            return QStringLiteral("Неизвестная ошибка");
        }
    }));
}
