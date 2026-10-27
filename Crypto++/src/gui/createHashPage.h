#pragma once
#include <memory>
#include <QApplication>
#include <QClipboard>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include "AppContext.h"
#include "createButton.h"
#include "createCard.h"
#include "createFileRow.h"
#include "createPageHeader.h"
#include "runTask.h"
#include "showWarning.h"
#include "toPath.h"
#include "../core/HashFile.h"

inline QWidget* createHashPage(AppContext* ctx) {
    auto* page = new QWidget;
    auto* root = new QVBoxLayout(page);
    root->setContentsMargins(0, 14, 0, 10);
    root->setSpacing(14);
    root->addWidget(createPageHeader(QStringLiteral("SHA-256"),
        QStringLiteral("Контрольная сумма файла. Файл читается потоком, поэтому размер не ограничен. Результат — в двоичном виде.")));

    Card file = createCard(QStringLiteral("Файл"));
    FileRow in = createFileRow(QStringLiteral("Файл для хеширования"), QStringLiteral("Выберите или перетащите файл"), FileRowMode::OpenFile);
    file.body->addWidget(in.widget);
    auto* computeBtn = createButton(QStringLiteral("Вычислить SHA-256"), ButtonKind::Primary);
    auto* row = new QHBoxLayout;
    row->addStretch(1);
    row->addWidget(computeBtn);
    file.body->addLayout(row);
    root->addWidget(file.frame);

    Card res = createCard(QStringLiteral("Результат"));
    auto* result = new QPlainTextEdit;
    result->setReadOnly(true);
    result->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    result->setMinimumHeight(190);
    result->setLineWrapMode(QPlainTextEdit::NoWrap);
    result->setPlaceholderText(QStringLiteral("Здесь появится хеш…"));
    auto* copyBtn = createButton(QStringLiteral("Копировать"));
    auto* copyRow = new QHBoxLayout;
    copyRow->addStretch(1);
    copyRow->addWidget(copyBtn);
    res.body->addWidget(result);
    res.body->addLayout(copyRow);
    root->addWidget(res.frame);
    root->addStretch(1);

    QObject::connect(copyBtn, &QPushButton::clicked, page, [=] {
        QApplication::clipboard()->setText(result->toPlainText());
        Log("Результат скопирован в буфер обмена");
    });

    QObject::connect(computeBtn, &QPushButton::clicked, page, [=] {
        if (in.edit->text().isEmpty()) {
            showWarning(page, QStringLiteral("Выберите файл."));
            return;
        }
        const auto f = toPath(in.edit->text());
        auto text = std::make_shared<std::string>();
        runTask(ctx, QStringLiteral("SHA-256"),
            [=] { *text = HashFile(f); },
            [=] { result->setPlainText(QString::fromStdString(*text)); });
    });
    return page;
}
