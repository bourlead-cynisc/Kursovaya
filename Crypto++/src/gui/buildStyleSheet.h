#pragma once
#include <QHash>
#include <QString>

// QSS-шаблон. Плейсхолдеры @{имя} заменяются значениями выбранной темы.
inline QString buildStyleSheet(const QHash<QString, QString>& t) {
    QString css = QStringLiteral(R"(
QWidget { color: @{text}; font-family: "Segoe UI", "Inter", "Noto Sans", sans-serif; font-size: 10pt; }
QMainWindow, QWidget#root { background: @{bg}; }
QScrollArea, QScrollArea > QWidget > QWidget { background: transparent; border: none; }

/* --- боковая панель --- */
QFrame#sidebar { background: @{sidebar}; border: none; border-right: 1px solid @{border}; }
QLabel#appTitle { font-size: 16pt; font-weight: 700; }
QLabel#sectionLabel { color: @{muted}; font-size: 8pt; font-weight: 700; letter-spacing: 1px; }
QPushButton[nav="true"] { text-align: left; padding: 11px 14px; border: none; border-radius: 10px;
    background: transparent; color: @{muted}; font-weight: 600; }
QPushButton[nav="true"]:hover { background: @{surface2}; color: @{text}; }
QPushButton[nav="true"]:checked { background: @{accentSoft}; color: @{accent}; }

/* --- типографика --- */
QLabel { background: transparent; }
QLabel#pageTitle { font-size: 21pt; font-weight: 700; }
QLabel#cardTitle { font-size: 11.5pt; font-weight: 600; }
QLabel#fieldLabel { color: @{muted}; font-size: 9pt; font-weight: 600; }
QLabel#muted { color: @{muted}; }

/* --- карточки --- */
QFrame#card { background: @{surface}; border: 1px solid @{border}; border-radius: 14px; }

/* --- поля ввода --- */
QLineEdit, QComboBox, QPlainTextEdit { background: @{surface2}; border: 1px solid @{border};
    border-radius: 9px; padding: 8px 11px; selection-background-color: @{accent}; selection-color: @{onAccent}; }
QLineEdit:focus, QComboBox:focus, QPlainTextEdit:focus { border: 1px solid @{accent}; }
QComboBox { min-height: 20px; }
QComboBox::drop-down { border: none; width: 28px; }
QComboBox QAbstractItemView { background: @{surface}; border: 1px solid @{border}; outline: 0; padding: 4px;
    selection-background-color: @{accentSoft}; selection-color: @{accent}; }
QPlainTextEdit#log { font-family: "Cascadia Mono", "JetBrains Mono", "DejaVu Sans Mono", monospace; font-size: 9pt; }

/* --- кнопки --- */
QPushButton { background: @{surface2}; border: 1px solid @{border}; border-radius: 9px;
    padding: 8px 18px; font-weight: 600; }
QPushButton:hover { border-color: @{accent}; }
QPushButton:pressed { background: @{border}; }
QPushButton:disabled { color: @{muted}; border-color: @{border}; }
QPushButton[variant="primary"] { background: @{accent}; color: @{onAccent}; border: 1px solid @{accent}; }
QPushButton[variant="primary"]:hover { background: @{accentHover}; border-color: @{accentHover}; }
QPushButton[variant="primary"]:disabled { background: @{surface2}; color: @{muted}; border-color: @{border}; }
QPushButton[variant="ghost"] { background: transparent; border: none; color: @{muted}; padding: 8px 12px; text-align: left; }
QPushButton[variant="ghost"]:hover { background: @{surface2}; color: @{text}; }

/* --- плашка результата --- */
QLabel#banner { border-radius: 10px; padding: 11px 16px; font-weight: 600; }
QLabel#banner[state="ok"]  { background: @{successSoft}; color: @{success}; }
QLabel#banner[state="bad"] { background: @{dangerSoft};  color: @{danger}; }

/* --- прогресс, разделитель, скроллбары --- */
QProgressBar { background: transparent; border: none; max-height: 3px; min-height: 3px; }
QProgressBar::chunk { background: @{accent}; border-radius: 1px; }
QSplitter::handle { background: transparent; }
QScrollBar:vertical { background: transparent; width: 10px; margin: 2px; }
QScrollBar::handle:vertical { background: @{border}; border-radius: 4px; min-height: 30px; }
QScrollBar::handle:vertical:hover { background: @{muted}; }
QScrollBar::add-line, QScrollBar::sub-line, QScrollBar::add-page, QScrollBar::sub-page { background: none; border: none; height: 0; }
QToolTip { background: @{surface}; color: @{text}; border: 1px solid @{border}; padding: 4px 8px; }
QMessageBox, QDialog { background: @{surface}; }
)");
    for (auto it = t.constBegin(); it != t.constEnd(); ++it)
        css.replace(QStringLiteral("@{") + it.key() + QStringLiteral("}"), it.value());
    return css;
}
