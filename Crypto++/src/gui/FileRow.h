#pragma once
class QWidget;
class QPushButton;
class FileLineEdit;

struct FileRow {
    QWidget*      widget = nullptr;
    FileLineEdit* edit   = nullptr;
    QPushButton*  button = nullptr;
};
