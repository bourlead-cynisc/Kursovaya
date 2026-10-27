#pragma once
#include <QObject>

class QMainWindow;
class QPlainTextEdit;
class QProgressBar;
class QWidget;
class QLabel;

// Общее состояние приложения (заменяет глобальные HWND/переменные из WinAPI-версии).
struct AppContext : QObject {
    QMainWindow*    window      = nullptr;
    QPlainTextEdit* logView     = nullptr;
    QProgressBar*   progress    = nullptr;
    QWidget*        content     = nullptr;   // блокируется на время операции
    QLabel*         statusLabel = nullptr;
    bool            busy        = false;
    bool            darkTheme   = true;

    explicit AppContext(QObject* parent = nullptr) : QObject(parent) {}
};
