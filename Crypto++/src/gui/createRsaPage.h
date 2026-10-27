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
#include "runTask.h"
#include "showWarning.h"
#include "toPath.h"
#include "toQString.h"
#include "../core/fileExists.h"
#include "../core/RSAGenerateKeys.h"
#include "../core/RSAEncrypt.h"
#include "../core/RSADecrypt.h"

inline QWidget* createRsaPage(AppContext* ctx) {
    auto* page = new QWidget;
    auto* root = new QVBoxLayout(page);
    root->setContentsMargins(0, 14, 0, 10);
    root->setSpacing(14);
    root->addWidget(createPageHeader(QStringLiteral("RSA"),
        QStringLiteral("Гибридное шифрование: файл шифруется AES-256, а ключ AES — RSA-OAEP.")));

    Card files = createCard(QStringLiteral("Файлы"),
        QStringLiteral("В папке ключей используются файлы private_<размер>.key и public_<размер>.key."));
    FileRow in   = createFileRow(QStringLiteral("Входной файл"), QStringLiteral("Что шифруем / расшифровываем"), FileRowMode::OpenFile);
    FileRow out  = createFileRow(QStringLiteral("Папка результата"), QStringLiteral("Куда сохранить результат"), FileRowMode::Folder);
    FileRow keys = createFileRow(QStringLiteral("Папка с ключами"), QStringLiteral("Где лежат (или будут созданы) ключи"), FileRowMode::Folder);
    files.body->addWidget(in.widget);
    files.body->addWidget(out.widget);
    files.body->addWidget(keys.widget);
    root->addWidget(files.frame);

    Card params = createCard(QStringLiteral("Параметры"));
    auto* sizeLabel = new QLabel(QStringLiteral("Размер ключа"));
    sizeLabel->setObjectName("fieldLabel");
    auto* combo = new QComboBox;
    for (int bits : { 1024, 2048, 4096, 8192, 16384 })
        combo->addItem(QStringLiteral("%1 бит").arg(bits), bits);
    combo->setCurrentIndex(1);
    combo->setMaximumWidth(220);
    auto* hint = new QLabel(QStringLiteral("Генерация ключей 8192 бит и больше может занять несколько минут."));
    hint->setObjectName("muted");
    hint->setWordWrap(true);
    hint->setVisible(false);
    QObject::connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), page,
        [=](int) { hint->setVisible(combo->currentData().toInt() >= 8192); });
    auto* comboCol = new QVBoxLayout;
    comboCol->setSpacing(6);
    comboCol->addWidget(sizeLabel);
    comboCol->addWidget(combo);

    auto* genBtn = createButton(QStringLiteral("Сгенерировать ключи"));
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
    params.body->addWidget(hint);
    root->addWidget(params.frame);
    root->addStretch(1);

    QObject::connect(in.edit, &QLineEdit::textChanged, page, [=](const QString& p) {
        if (out.edit->text().isEmpty() && QFileInfo(p).isFile())
            out.edit->setText(QDir::toNativeSeparators(QFileInfo(p).absolutePath()));
    });

    QObject::connect(genBtn, &QPushButton::clicked, page, [=] {
        if (keys.edit->text().isEmpty()) {
            showWarning(page, QStringLiteral("Сначала выберите папку с ключами."));
            return;
        }
        const int bits = combo->currentData().toInt();
        const auto dir = toPath(keys.edit->text());
        runTask(ctx, QStringLiteral("Генерация RSA-%1").arg(bits),
            [=] { RSAGenerateKeys(bits, dir); },
            [=] { Log("Ключи сохранены в: " + keys.edit->text().toStdString()); });
    });

    auto validate = [=]() {
        if (in.edit->text().isEmpty() || out.edit->text().isEmpty() || keys.edit->text().isEmpty()) {
            showWarning(page, QStringLiteral("Заполните все поля: входной файл, папка результата и папка ключей."));
            return false;
        }
        return true;
    };

    QObject::connect(encBtn, &QPushButton::clicked, page, [=] {
        if (!validate()) return;
        const int bits = combo->currentData().toInt();
        const auto pub = toPath(keys.edit->text()) / ("public_" + std::to_string(bits) + ".key");
        if (!fileExists(pub)) {
            showWarning(page, QStringLiteral("Открытый ключ не найден. Сначала сгенерируйте ключи."));
            return;
        }
        const auto i = toPath(in.edit->text()), o = toPath(out.edit->text());
        auto result = std::make_shared<std::filesystem::path>();
        runTask(ctx, QStringLiteral("RSA-%1: шифрование").arg(bits),
            [=] { *result = RSAEncrypt(i, o, pub, bits); },
            [=] { Log("Результат: " + toQString(*result).toStdString()); });
    });

    QObject::connect(decBtn, &QPushButton::clicked, page, [=] {
        if (!validate()) return;
        const int bits = combo->currentData().toInt();
        const auto priv = toPath(keys.edit->text()) / ("private_" + std::to_string(bits) + ".key");
        if (!fileExists(priv)) {
            showWarning(page, QStringLiteral("Закрытый ключ не найден. Сначала сгенерируйте ключи."));
            return;
        }
        const auto i = toPath(in.edit->text()), o = toPath(out.edit->text());
        auto result = std::make_shared<std::filesystem::path>();
        runTask(ctx, QStringLiteral("RSA-%1: расшифровка").arg(bits),
            [=] { *result = RSADecrypt(i, o, priv, bits); },
            [=] { Log("Результат: " + toQString(*result).toStdString()); });
    });
    return page;
}
