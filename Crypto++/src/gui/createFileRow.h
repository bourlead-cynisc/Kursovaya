#pragma once
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include "FileRow.h"
#include "FileRowMode.h"
#include "FileLineEdit.h"
#include "createButton.h"
#include "browseForFile.h"
#include "browseForFolder.h"

// Подпись + поле пути + кнопка «Обзор» (замена STATIC + EDIT + BUTTON из WinAPI).
inline FileRow createFileRow(const QString& label, const QString& placeholder, FileRowMode mode,
                             const QString& filter = QStringLiteral("Все файлы (*)")) {
    auto* w = new QWidget;
    auto* v = new QVBoxLayout(w);
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(6);

    auto* l = new QLabel(label);
    l->setObjectName("fieldLabel");

    auto* edit = new FileLineEdit;
    edit->setPlaceholderText(placeholder);
    auto* btn = createButton(mode == FileRowMode::Folder ? QStringLiteral("Выбрать…")
                                                         : QStringLiteral("Обзор…"));

    auto* h = new QHBoxLayout;
    h->setSpacing(8);
    h->addWidget(edit, 1);
    h->addWidget(btn);
    v->addWidget(l);
    v->addLayout(h);

    QObject::connect(btn, &QPushButton::clicked, w, [=] {
        const QString r = (mode == FileRowMode::Folder) ? browseForFolder(w, label)
                                                        : browseForFile(w, label, filter);
        if (!r.isEmpty()) edit->setText(r);
    });
    return { w, edit, btn };
}
