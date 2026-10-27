#pragma once
#include <memory>
#include <QFileInfo>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include "AppContext.h"
#include "GostCipherPageConfig.h"
#include "browseForSaveFile.h"
#include "createButton.h"
#include "createCard.h"
#include "createFileRow.h"
#include "createPageHeader.h"
#include "runTask.h"
#include "showWarning.h"
#include "toPath.h"
#include "toQString.h"
#include "../core/Log.h"
#include "../core/formatFileSize.h"
#include "../core/pathToUtf8.h"
#include "../core/toBinarySection.h"

// Общая страница для Магмы и Кузнечика (оба: ключ 256 бит, режим CBC, данные показываются в двоичном виде).
inline QWidget* createGostCipherPage(AppContext* ctx, const GostCipherPageConfig& cfg) {
    auto* page = new QWidget;
    auto* root = new QVBoxLayout(page);
    root->setContentsMargins(0, 14, 0, 10);
    root->setSpacing(14);
    root->addWidget(createPageHeader(cfg.title, cfg.subtitle));

    Card files = createCard(QStringLiteral("Файлы и ключ"));
    FileRow in  = createFileRow(QStringLiteral("Входной файл"), QStringLiteral("Что шифруем / расшифровываем"), FileRowMode::OpenFile);
    FileRow out = createFileRow(QStringLiteral("Папка результата"), QStringLiteral("Куда сохранить результат"), FileRowMode::Folder);
    FileRow key = createFileRow(QStringLiteral("Файл ключа"), QStringLiteral("*.key (создайте кнопкой «Сгенерировать ключ»)"),
                                FileRowMode::OpenFile, QStringLiteral("Ключи (*.key);;Все файлы (*)"));
    files.body->addWidget(in.widget);
    files.body->addWidget(out.widget);
    files.body->addWidget(key.widget);

    auto* genBtn = createButton(QStringLiteral("Сгенерировать ключ"));
    auto* decBtn = createButton(QStringLiteral("Расшифровать"));
    auto* encBtn = createButton(QStringLiteral("Зашифровать"), ButtonKind::Primary);
    auto* actions = new QHBoxLayout;
    actions->setSpacing(10);
    actions->addWidget(genBtn);
    actions->addStretch(1);
    actions->addWidget(decBtn);
    actions->addWidget(encBtn);
    files.body->addLayout(actions);
    root->addWidget(files.frame);

    Card res = createCard(QStringLiteral("Данные в двоичном виде"));
    auto* result = new QPlainTextEdit;
    result->setReadOnly(true);
    result->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    result->setMinimumHeight(245);
    result->setLineWrapMode(QPlainTextEdit::NoWrap);
    result->setPlaceholderText(QStringLiteral("Здесь появятся ключ, вектор инициализации и начало шифртекста…"));
    res.body->addWidget(result);
    root->addWidget(res.frame);
    root->addStretch(1);

    QObject::connect(in.edit, &QLineEdit::textChanged, page, [=](const QString& p) {
        if (out.edit->text().isEmpty() && QFileInfo(p).isFile())
            out.edit->setText(QDir::toNativeSeparators(QFileInfo(p).absolutePath()));
    });

    // --- генерация ключа ---
    QObject::connect(genBtn, &QPushButton::clicked, page, [=] {
        const QString f = browseForSaveFile(page, QStringLiteral("Сохранить ключ"),
            QStringLiteral("Ключи (*.key)"), cfg.defaultKeyName);
        if (f.isEmpty()) return;
        auto k = std::make_shared<std::vector<uint8_t>>();
        runTask(ctx, QStringLiteral("%1: генерация ключа").arg(cfg.title),
            [=] { *k = cfg.generateKey(toPath(f)); },
            [=] {
                key.edit->setText(f);
                Log("Ключ сохранён: " + f.toStdString());
                result->setPlainText(QString::fromStdString(
                    "Сгенерирован новый ключ\n\n" + toBinarySection("Ключ", k->data(), k->size())));
            });
    });

    auto validate = [=]() {
        if (in.edit->text().isEmpty() || out.edit->text().isEmpty() || key.edit->text().isEmpty()) {
            showWarning(page, QStringLiteral("Заполните все поля: входной файл, папка результата и ключ."));
            return false;
        }
        return true;
    };

    // --- шифрование / расшифровка: общий код, отличаются функция и подписи ---
    auto runOperation = [=](bool encrypt) {
        if (!validate()) return;
        const auto i = toPath(in.edit->text()), o = toPath(out.edit->text()), k = toPath(key.edit->text());
        auto info = std::make_shared<CipherRunInfo>();
        const QString what = encrypt ? QStringLiteral("шифрование") : QStringLiteral("расшифровка");
        runTask(ctx, QStringLiteral("%1: %2").arg(cfg.title, what),
            [=] { *info = encrypt ? cfg.encrypt(i, o, k) : cfg.decrypt(i, o, k); },
            [=] {
                Log("Результат: " + toQString(info->outputFile).toStdString());
                std::string text = std::string("Операция: ") + (encrypt ? "шифрование" : "расшифровка") + "\n" +
                    "Результат: " + pathToUtf8(info->outputFile.filename()) + "\n" +
                    "Размер: " + formatFileSize(info->inputSize) + " → " + formatFileSize(info->outputSize) + "\n\n" +
                    toBinarySection("Вектор инициализации (IV)", info->iv.data(), info->iv.size()) + "\n" +
                    toBinarySection(encrypt ? "Первые байты шифртекста" : "Первые байты открытого текста",
                                    info->preview.data(), info->preview.size());
                result->setPlainText(QString::fromStdString(text));
            });
    };
    QObject::connect(encBtn, &QPushButton::clicked, page, [=] { runOperation(true); });
    QObject::connect(decBtn, &QPushButton::clicked, page, [=] { runOperation(false); });
    return page;
}
