#pragma once
#include <memory>
#include <QComboBox>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include "AppContext.h"
#include "createButton.h"
#include "createCard.h"
#include "createFileRow.h"
#include "createPageHeader.h"
#include "browseForSaveFile.h"
#include "runTask.h"
#include "showWarning.h"
#include "toPath.h"
#include "toQString.h"
#include "../core/AESGenerateKey.h"
#include "../core/AESEncrypt.h"
#include "../core/AESDecrypt.h"

inline QWidget* createAesPage(AppContext* ctx) {
    auto* page = new QWidget;
    auto* root = new QVBoxLayout(page);
    root->setContentsMargins(0, 14, 0, 10);
    root->setSpacing(14);
    root->addWidget(createPageHeader(QStringLiteral("AES"),
        QStringLiteral("Симметричное шифрование файлов: AES-CBC с PKCS#7 и случайным IV.")));

    Card files = createCard(QStringLiteral("Файлы"),
        QStringLiteral("Выберите файл кнопкой или перетащите его в поле."));
    FileRow in  = createFileRow(QStringLiteral("Входной файл"), QStringLiteral("Что шифруем / расшифровываем"), FileRowMode::OpenFile);
    FileRow out = createFileRow(QStringLiteral("Папка результата"), QStringLiteral("Куда сохранить результат"), FileRowMode::Folder);
    FileRow key = createFileRow(QStringLiteral("Файл ключа"), QStringLiteral("*.key (создайте кнопкой «Сгенерировать ключ»)"),
                                FileRowMode::OpenFile, QStringLiteral("Ключи (*.key);;Все файлы (*)"));
    files.body->addWidget(in.widget);
    files.body->addWidget(out.widget);
    files.body->addWidget(key.widget);
    root->addWidget(files.frame);

    Card params = createCard(QStringLiteral("Параметры"));
    auto* sizeLabel = new QLabel(QStringLiteral("Размер ключа"));
    sizeLabel->setObjectName("fieldLabel");
    auto* combo = new QComboBox;
    combo->addItem(QStringLiteral("128 бит"), 128);
    combo->addItem(QStringLiteral("192 бита"), 192);
    combo->addItem(QStringLiteral("256 бит"), 256);
    combo->setCurrentIndex(2);
    combo->setMaximumWidth(220);
    auto* comboCol = new QVBoxLayout;
    comboCol->setSpacing(6);
    comboCol->addWidget(sizeLabel);
    comboCol->addWidget(combo);

    auto* genBtn = createButton(QStringLiteral("Сгенерировать ключ"));
    auto* decBtn = createButton(QStringLiteral("Расшифровать"));
    auto* encBtn = createButton(QStringLiteral("Зашифровать"), ButtonKind::Primary);
    auto* actions = new QHBoxLayout;
    actions->setSpacing(10);
    actions->addLayout(comboCol);
    actions->addStretch(1);
    actions->addWidget(genBtn, 0, Qt::AlignBottom);
    actions->addWidget(decBtn, 0, Qt::AlignBottom);
    actions->addWidget(encBtn, 0, Qt::AlignBottom);
    params.body->addLayout(actions);
    root->addWidget(params.frame);
    root->addStretch(1);

    // Удобства: папка результата по умолчанию = папка входного файла; размер ключа берём из длины файла ключа.
    QObject::connect(in.edit, &QLineEdit::textChanged, page, [=](const QString& p) {
        if (out.edit->text().isEmpty() && QFileInfo(p).isFile())
            out.edit->setText(QDir::toNativeSeparators(QFileInfo(p).absolutePath()));
    });
    QObject::connect(key.edit, &QLineEdit::textChanged, page, [=](const QString& p) {
        const qint64 s = QFileInfo(p).size();
        if (s == 16) combo->setCurrentIndex(0);
        else if (s == 24) combo->setCurrentIndex(1);
        else if (s == 32) combo->setCurrentIndex(2);
    });

    QObject::connect(genBtn, &QPushButton::clicked, page, [=] {
        const int bits = combo->currentData().toInt();
        const QString f = browseForSaveFile(page, QStringLiteral("Сохранить ключ"),
            QStringLiteral("Ключи (*.key)"), QStringLiteral("aes_%1.key").arg(bits));
        if (f.isEmpty()) return;
        runTask(ctx, QStringLiteral("Генерация ключа AES-%1").arg(bits),
            [=] { AESGenerateKey(bits, toPath(f)); },
            [=] { key.edit->setText(f); Log("Ключ сохранён: " + f.toStdString()); });
    });

    auto validate = [=]() {
        if (in.edit->text().isEmpty() || out.edit->text().isEmpty() || key.edit->text().isEmpty()) {
            showWarning(page, QStringLiteral("Заполните все поля: входной файл, папка результата и ключ."));
            return false;
        }
        return true;
    };

    QObject::connect(encBtn, &QPushButton::clicked, page, [=] {
        if (!validate()) return;
        const int bits = combo->currentData().toInt();
        const auto i = toPath(in.edit->text()), o = toPath(out.edit->text()), k = toPath(key.edit->text());
        auto result = std::make_shared<std::filesystem::path>();
        runTask(ctx, QStringLiteral("AES-%1: шифрование").arg(bits),
            [=] { *result = AESEncrypt(i, o, k, bits); },
            [=] { Log("Результат: " + toQString(*result).toStdString()); });
    });

    QObject::connect(decBtn, &QPushButton::clicked, page, [=] {
        if (!validate()) return;
        const int bits = combo->currentData().toInt();
        const auto i = toPath(in.edit->text()), o = toPath(out.edit->text()), k = toPath(key.edit->text());
        auto result = std::make_shared<std::filesystem::path>();
        runTask(ctx, QStringLiteral("AES-%1: расшифровка").arg(bits),
            [=] { *result = AESDecrypt(i, o, k, bits); },
            [=] { Log("Результат: " + toQString(*result).toStdString()); });
    });
    return page;
}
