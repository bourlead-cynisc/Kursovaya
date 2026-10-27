"""Тема оформления: бирюзовая палитра, тёмная и светлая. Шаблон QSS с плейсхолдерами @{имя}."""
from PySide6.QtGui import QColor, QPalette
from PySide6.QtWidgets import QApplication

DARK = {
    "bg": "#0b1114", "header": "#0f171b", "surface": "#121d22", "surface2": "#182730", "border": "#223641",
    "text": "#e4eef1", "muted": "#7f96a0", "accent": "#2dd4bf", "accentHover": "#5eead4", "onAccent": "#04201c",
    "accentSoft": "rgba(45,212,191,36)", "success": "#4ade80", "successSoft": "rgba(74,222,128,34)",
    "danger": "#fb7185", "dangerSoft": "rgba(251,113,133,34)", "console": "#080d0f", "consoleText": "#9be7d8",
}
LIGHT = {
    "bg": "#eef4f5", "header": "#ffffff", "surface": "#ffffff", "surface2": "#f0f6f7", "border": "#d5e2e5",
    "text": "#10252b", "muted": "#587077", "accent": "#0f766e", "accentHover": "#0d9488", "onAccent": "#ffffff",
    "accentSoft": "rgba(15,118,110,30)", "success": "#15803d", "successSoft": "rgba(21,128,61,28)",
    "danger": "#be123c", "dangerSoft": "rgba(190,18,60,26)", "console": "#0f1e23", "consoleText": "#b7efe3",
}

_QSS = """
QWidget { color: @{text}; font-family: "Segoe UI", "Inter", "Noto Sans", sans-serif; font-size: 10pt; }
QMainWindow, QWidget#root { background: @{bg}; }
QScrollArea, QScrollArea > QWidget > QWidget { background: transparent; border: none; }

/* --- верхняя панель --- */
QFrame#header { background: @{header}; border: none; border-bottom: 1px solid @{border}; }
QLabel#brand { font-size: 14pt; font-weight: 700; color: @{text}; }
QLabel#brandAccent { font-size: 14pt; font-weight: 700; color: @{accent}; }
QPushButton[nav="true"] { background: transparent; border: none; border-radius: 15px; padding: 7px 16px;
    color: @{muted}; font-weight: 600; }
QPushButton[nav="true"]:hover { background: @{surface2}; color: @{text}; }
QPushButton[nav="true"]:checked { background: @{accent}; color: @{onAccent}; }

/* --- типографика --- */
QLabel { background: transparent; }
QLabel#pageTitle { font-size: 20pt; font-weight: 700; }
QLabel#cardTitle { font-size: 11pt; font-weight: 600; }
QLabel#fieldLabel { color: @{muted}; font-size: 9pt; font-weight: 600; }
QLabel#muted { color: @{muted}; }

/* --- карточки --- */
QFrame#card { background: @{surface}; border: 1px solid @{border}; border-radius: 16px; }
QFrame#cardBar { background: @{accent}; border: none; border-radius: 2px; }

/* --- поля --- */
QLineEdit, QComboBox, QPlainTextEdit { background: @{surface2}; border: 1px solid @{border}; border-radius: 10px;
    padding: 8px 11px; selection-background-color: @{accent}; selection-color: @{onAccent}; }
QLineEdit:focus, QComboBox:focus, QPlainTextEdit:focus { border: 1px solid @{accent}; }
QComboBox { min-height: 20px; }
QComboBox::drop-down { border: none; width: 28px; }
QComboBox QAbstractItemView { background: @{surface}; border: 1px solid @{border}; outline: 0; padding: 4px;
    selection-background-color: @{accentSoft}; selection-color: @{accent}; }

/* --- кнопки --- */
QPushButton { background: @{surface2}; border: 1px solid @{border}; border-radius: 10px; padding: 8px 18px; font-weight: 600; }
QPushButton:hover { border-color: @{accent}; }
QPushButton:pressed { background: @{border}; }
QPushButton:disabled { color: @{muted}; }
QPushButton[variant="primary"] { background: @{accent}; color: @{onAccent}; border: 1px solid @{accent}; }
QPushButton[variant="primary"]:hover { background: @{accentHover}; border-color: @{accentHover}; }
QPushButton[variant="primary"]:disabled { background: @{surface2}; color: @{muted}; border-color: @{border}; }
QPushButton[variant="ghost"] { background: transparent; border: none; color: @{muted}; padding: 6px 12px; }
QPushButton[variant="ghost"]:hover { background: @{surface2}; color: @{text}; }

/* --- плашка результата --- */
QLabel#banner { border-radius: 12px; padding: 11px 16px; font-weight: 600; }
QLabel#banner[state="ok"]  { background: @{successSoft}; color: @{success}; }
QLabel#banner[state="bad"] { background: @{dangerSoft};  color: @{danger}; }

/* --- журнал-консоль справа --- */
QFrame#logPanel { background: @{console}; border: none; border-left: 1px solid @{border}; }
QLabel#logTitle { color: @{consoleText}; font-weight: 700; letter-spacing: 1px; }
QPlainTextEdit#console { background: transparent; border: none; color: @{consoleText}; padding: 2px;
    font-family: "Cascadia Mono", "JetBrains Mono", "DejaVu Sans Mono", monospace; font-size: 9pt; }
QPushButton#logClear { background: transparent; border: 1px solid #2a4a50; color: @{consoleText}; padding: 4px 12px; border-radius: 8px; }
QPushButton#logClear:hover { border-color: @{consoleText}; }

/* --- строка состояния --- */
QFrame#footer { background: @{header}; border: none; border-top: 1px solid @{border}; }
QLabel#status { color: @{muted}; }
QProgressBar { background: @{surface2}; border: none; border-radius: 3px; max-height: 6px; min-height: 6px; }
QProgressBar::chunk { background: @{accent}; border-radius: 3px; }

QSplitter::handle { background: transparent; }
QScrollBar:vertical { background: transparent; width: 10px; margin: 2px; }
QScrollBar::handle:vertical { background: @{border}; border-radius: 4px; min-height: 30px; }
QScrollBar::handle:vertical:hover { background: @{muted}; }
QScrollBar::add-line, QScrollBar::sub-line, QScrollBar::add-page, QScrollBar::sub-page { background: none; border: none; height: 0; }
QScrollBar:horizontal { background: transparent; height: 10px; margin: 2px; }
QScrollBar::handle:horizontal { background: @{border}; border-radius: 4px; min-width: 30px; }
QToolTip { background: @{surface}; color: @{text}; border: 1px solid @{border}; padding: 4px 8px; }
QMessageBox, QDialog { background: @{surface}; }
"""


def build_stylesheet(tokens: dict) -> str:
    css = _QSS
    for key, value in tokens.items():
        css = css.replace("@{" + key + "}", value)
    return css


def build_palette(tokens: dict) -> QPalette:
    p = QPalette()
    p.setColor(QPalette.Window, QColor(tokens["bg"]))
    p.setColor(QPalette.WindowText, QColor(tokens["text"]))
    p.setColor(QPalette.Base, QColor(tokens["surface2"]))
    p.setColor(QPalette.AlternateBase, QColor(tokens["surface"]))
    p.setColor(QPalette.Text, QColor(tokens["text"]))
    p.setColor(QPalette.Button, QColor(tokens["surface2"]))
    p.setColor(QPalette.ButtonText, QColor(tokens["text"]))
    p.setColor(QPalette.ToolTipBase, QColor(tokens["surface"]))
    p.setColor(QPalette.ToolTipText, QColor(tokens["text"]))
    p.setColor(QPalette.PlaceholderText, QColor(tokens["muted"]))
    p.setColor(QPalette.Highlight, QColor(tokens["accent"]))
    p.setColor(QPalette.HighlightedText, QColor(tokens["onAccent"]))
    return p


def apply_theme(dark: bool) -> None:
    tokens = DARK if dark else LIGHT
    app = QApplication.instance()
    app.setPalette(build_palette(tokens))
    app.setStyleSheet(build_stylesheet(tokens))
