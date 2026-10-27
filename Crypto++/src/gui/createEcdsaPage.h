#pragma once
#include <memory>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include "AppContext.h"
#include "createButton.h"
#include "createCard.h"
#include "createFileRow.h"
#include "createPageHeader.h"
#include "browseForFile.h"
#include "browseForSaveFile.h"
#include "runTask.h"
#include "showBanner.h"
#include "showWarning.h"
#include "toPath.h"
#include "toQString.h"
#include "../core/ecdsaPublicKeyPathFor.h"
#include "../core/ECDSAGenerateKeys.h"
#include "../core/ECDSASign.h"
#include "../core/ECDSAVerify.h"

inline QWidget* createEcdsaPage(AppContext* ctx) {
    auto* page = new QWidget;
    auto* root = new QVBoxLayout(page);
    root->setContentsMargins(0, 14, 0, 10);
    root->setSpacing(14);
    root->addWidget(createPageHeader(QStringLiteral("ECDSA"),
        QStringLiteral("Цифровая подпись файла на эллиптических кривых (подписывается SHA-256 файла).")));

    const QString keyFilter = QStringLiteral("Ключи (*.key);;Все файлы (*)");
    Card files = createCard(QStringLiteral("Файлы и ключи"));
    FileRow in   = createFileRow(QStringLiteral("Файл"), QStringLiteral("Что подписываем / проверяем"), FileRowMode::OpenFile);
    FileRow priv = createFileRow(QStringLiteral("Закрытый ключ (для подписи)"), QStringLiteral("*.key"), FileRowMode::OpenFile, keyFilter);
    FileRow pub  = createFileRow(QStringLiteral("Открытый ключ (для проверки)"), QStringLiteral("*_pub.key"), FileRowMode::OpenFile, keyFilter);
    files.body->addWidget(in.widget);
    files.body->addWidget(priv.widget);
    files.body->addWidget(pub.widget);
    root->addWidget(files.frame);

    Card params = createCard(QStringLiteral("Действия"));
    auto* curveLabel = new QLabel(QStringLiteral("Кривая (для генерации ключей)"));
    curveLabel->setObjectName("fieldLabel");
    auto* combo = new QComboBox;
    for (const char* c : { "P-256", "P-384", "P-521" }) combo->addItem(c, c);
    combo->setMaximumWidth(220);
    auto* comboCol = new QVBoxLayout;
    comboCol->setSpacing(6);
    comboCol->addWidget(curveLabel);
    comboCol->addWidget(combo);

    auto* genBtn    = createButton(QStringLiteral("Сгенерировать ключи"));
    auto* verifyBtn = createButton(QStringLiteral("Проверить подпись"));
    auto* signBtn   = createButton(QStringLiteral("Подписать"), ButtonKind::Primary);
    auto* actions = new QHBoxLayout;
    actions->setSpacing(10);
    actions->addLayout(comboCol);
    actions->addStretch(1);
    actions->addWidget(genBtn, 0, Qt::AlignBottom);
    actions->addWidget(verifyBtn, 0, Qt::AlignBottom);
    actions->addWidget(signBtn, 0, Qt::AlignBottom);
    params.body->addLayout(actions);
    root->addWidget(params.frame);

    auto* banner = new QLabel;
    banner->setObjectName("banner");
    banner->hide();
    root->addWidget(banner);
    root->addStretch(1);

    QObject::connect(in.edit, &QLineEdit::textChanged, page, [=] { banner->hide(); });

    QObject::connect(genBtn, &QPushButton::clicked, page, [=] {
        const std::string curve = combo->currentData().toString().toStdString();
        const QString privPath = browseForSaveFile(page, QStringLiteral("Сохранить закрытый ключ"),
            QStringLiteral("Ключи (*.key)"), QStringLiteral("ecdsa_private.key"));
        if (privPath.isEmpty()) return;
        const auto privFile = toPath(privPath);
        const auto pubFile = ecdsaPublicKeyPathFor(privFile);
        runTask(ctx, QStringLiteral("Генерация ECDSA (%1)").arg(QString::fromStdString(curve)),
            [=] { ECDSAGenerateKeys(curve, privFile, pubFile); },
            [=] {
                priv.edit->setText(privPath);
                pub.edit->setText(toQString(pubFile));
                Log("Закрытый ключ: " + privPath.toStdString());
                Log("Открытый ключ: " + toQString(pubFile).toStdString());
            });
    });

    QObject::connect(signBtn, &QPushButton::clicked, page, [=] {
        if (in.edit->text().isEmpty() || priv.edit->text().isEmpty()) {
            showWarning(page, QStringLiteral("Выберите файл и закрытый ключ."));
            return;
        }
        const QString sigPath = browseForSaveFile(page, QStringLiteral("Сохранить подпись"),
            QStringLiteral("Подписи (*.sig)"), QStringLiteral("signature.sig"));
        if (sigPath.isEmpty()) return;
        const auto i = toPath(in.edit->text()), k = toPath(priv.edit->text()), s = toPath(sigPath);
        runTask(ctx, QStringLiteral("ECDSA: подпись"),
            [=] { ECDSASign(i, k, s); },
            [=] {
                Log("Подпись сохранена: " + sigPath.toStdString());
                showBanner(banner, QStringLiteral("Подпись создана и сохранена"), "ok");
            });
    });

    QObject::connect(verifyBtn, &QPushButton::clicked, page, [=] {
        if (in.edit->text().isEmpty() || pub.edit->text().isEmpty()) {
            showWarning(page, QStringLiteral("Выберите файл и открытый ключ."));
            return;
        }
        const QString sigPath = browseForFile(page, QStringLiteral("Выберите файл подписи"),
            QStringLiteral("Подписи (*.sig);;Все файлы (*)"));
        if (sigPath.isEmpty()) return;
        const auto i = toPath(in.edit->text()), k = toPath(pub.edit->text()), s = toPath(sigPath);
        auto ok = std::make_shared<bool>(false);
        runTask(ctx, QStringLiteral("ECDSA: проверка"),
            [=] { *ok = ECDSAVerify(i, k, s); },
            [=] {
                if (*ok) showBanner(banner, QStringLiteral("✔  Подпись верна — файл не изменялся"), "ok");
                else     showBanner(banner, QStringLiteral("✖  Подпись НЕВЕРНА — файл изменён или ключ не тот"), "bad");
            });
    });
    return page;
}
