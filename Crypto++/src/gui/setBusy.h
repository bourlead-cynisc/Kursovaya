#pragma once
#include <QLabel>
#include <QProgressBar>
#include "AppContext.h"

inline void setBusy(AppContext* ctx, bool busy, const QString& message = QString()) {
    ctx->busy = busy;
    if (ctx->content)     ctx->content->setEnabled(!busy);
    if (ctx->progress)    ctx->progress->setRange(0, busy ? 0 : 1);   // 0..0 = бегущая полоса
    if (ctx->statusLabel) ctx->statusLabel->setText(busy ? message : QStringLiteral("Готово"));
}
