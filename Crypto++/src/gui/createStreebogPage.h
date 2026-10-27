#pragma once
#include <memory>
#include <QApplication>
#include <QClipboard>
#include <QComboBox>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QLabel>
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
#include "../core/streebog/HashFileStreebog.h"

inline QWidget* createStreebogPage(AppContext* ctx) {
    auto* page = new QWidget;
    auto* root = new QVBoxLayout(page);
    root->setContentsMargins(0, 14, 0, 10);
    root->setSpacing(14);
    root->addWidget(createPageHeader(QStringLiteral("Стрибог"),
        QStringLiteral("Хеш-функция ГОСТ Р 34.11-2012 с длиной хеша 256 или 512 бит. Результат — в двоичном виде.")));

    Card file = createCard(QStringLiteral("Файл"));
    FileRow in = createFileRow(QStringLiteral("Файл для хеширования"), QStringLiteral("Выберите или перетащите файл"), FileRowMode::OpenFile);
    file.body->addWidget(in.widget);

    auto* sizeLabel = new QLabel(QStringLiteral("Длина хеша"));
    sizeLabel->setObjectName("fieldLabel");
    auto* combo = new QComboBox;
    combo->addItem(QStringLiteral("256 бит"), 256);
    combo->addItem(QStringLiteral("512 бит"), 512);
    combo->setMaximumWidth(220);
    auto* comboCol = new QVBoxLayout;
    comboCol->setSpacing(6);
    comboCol->addWidget(sizeLabel);
    comboCol->addWidget(combo);
    auto* computeBtn = createButton(QStringLiteral("Вычислить хеш"), ButtonKind::Primary);
    auto* row = new QHBoxLayout;
    row->addLayout(comboCol);
    row->addStretch(1);
    row->addWidget(computeBtn, 0, Qt::AlignBottom);
    file.body->addLayout(row);
    root->addWidget(file.frame);

    Card res = createCard(QStringLiteral("Результат"));
    auto* result = new QPlainTextEdit;
    result->setReadOnly(true);
    result->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    result->setMinimumHeight(275);
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
        const int bits = combo->currentData().toInt();
        auto text = std::make_shared<std::string>();
        runTask(ctx, QStringLiteral("Стрибог-%1").arg(bits),
            [=] { *text = HashFileStreebog(f, bits); },
            [=] { result->setPlainText(QString::fromStdString(*text)); });
    });
    return page;
}
