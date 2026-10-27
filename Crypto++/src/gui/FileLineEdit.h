#pragma once
#include <QDir>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QLineEdit>
#include <QMimeData>
#include <QUrl>

// Поле пути: можно вставить путь руками или перетащить файл/папку из проводника.
class FileLineEdit : public QLineEdit {
public:
    explicit FileLineEdit(QWidget* parent = nullptr) : QLineEdit(parent) {
        setAcceptDrops(true);
        setClearButtonEnabled(true);
    }

protected:
    void dragEnterEvent(QDragEnterEvent* e) override {
        if (e->mimeData()->hasUrls()) e->acceptProposedAction();
        else QLineEdit::dragEnterEvent(e);
    }
    void dragMoveEvent(QDragMoveEvent* e) override {
        if (e->mimeData()->hasUrls()) e->acceptProposedAction();
        else QLineEdit::dragMoveEvent(e);
    }
    void dropEvent(QDropEvent* e) override {
        if (e->mimeData()->hasUrls()) {
            const QString path = e->mimeData()->urls().first().toLocalFile();
            if (!path.isEmpty()) setText(QDir::toNativeSeparators(path));
            e->acceptProposedAction();
        } else {
            QLineEdit::dropEvent(e);
        }
    }
};
