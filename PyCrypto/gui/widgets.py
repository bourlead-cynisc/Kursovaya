"""Базовые элементы интерфейса: поля путей, карточки, кнопки, диалоги."""
import os
from typing import Optional

from PySide6.QtCore import Qt
from PySide6.QtGui import QFontDatabase
from PySide6.QtWidgets import (QFileDialog, QFrame, QHBoxLayout, QLabel, QLineEdit, QMessageBox,
                               QPlainTextEdit, QPushButton, QVBoxLayout, QWidget)


def native(path: str) -> str:
    return os.path.normpath(path) if path else path


class FileLineEdit(QLineEdit):
    """Поле пути: можно вставить путь руками или перетащить файл/папку из проводника."""

    def __init__(self, parent=None) -> None:
        super().__init__(parent)
        self.setAcceptDrops(True)
        self.setClearButtonEnabled(True)

    def dragEnterEvent(self, e) -> None:
        if e.mimeData().hasUrls():
            e.acceptProposedAction()
        else:
            super().dragEnterEvent(e)

    def dragMoveEvent(self, e) -> None:
        if e.mimeData().hasUrls():
            e.acceptProposedAction()
        else:
            super().dragMoveEvent(e)

    def dropEvent(self, e) -> None:
        if e.mimeData().hasUrls():
            path = e.mimeData().urls()[0].toLocalFile()
            if path:
                self.setText(native(path))
            e.acceptProposedAction()
        else:
            super().dropEvent(e)


def make_button(text: str, kind: str = "secondary", parent=None) -> QPushButton:
    b = QPushButton(text, parent)
    b.setCursor(Qt.PointingHandCursor)
    b.setMinimumHeight(38)
    if kind in ("primary", "ghost"):
        b.setProperty("variant", kind)
    return b


class Card(QFrame):
    """Карточка с цветной полоской и заголовком; содержимое добавляется в self.body."""

    def __init__(self, title: str, subtitle: str = "", parent=None) -> None:
        super().__init__(parent)
        self.setObjectName("card")
        self.body = QVBoxLayout(self)
        self.body.setContentsMargins(22, 18, 22, 22)
        self.body.setSpacing(14)

        head = QHBoxLayout()
        head.setSpacing(10)
        bar = QFrame()
        bar.setObjectName("cardBar")
        bar.setFixedSize(4, 18)
        t = QLabel(title)
        t.setObjectName("cardTitle")
        head.addWidget(bar)
        head.addWidget(t)
        head.addStretch(1)
        self.body.addLayout(head)
        if subtitle:
            s = QLabel(subtitle)
            s.setObjectName("muted")
            s.setWordWrap(True)
            self.body.addWidget(s)


class PageHeader(QWidget):
    def __init__(self, title: str, subtitle: str, parent=None) -> None:
        super().__init__(parent)
        v = QVBoxLayout(self)
        v.setContentsMargins(0, 0, 0, 0)
        v.setSpacing(4)
        t = QLabel(title)
        t.setObjectName("pageTitle")
        s = QLabel(subtitle)
        s.setObjectName("muted")
        s.setWordWrap(True)
        v.addWidget(t)
        v.addWidget(s)


class FileRow(QWidget):
    """Подпись + поле пути + кнопка «Обзор». mode: "file" или "folder"."""

    def __init__(self, label: str, placeholder: str, mode: str = "file",
                 file_filter: str = "Все файлы (*)", parent=None) -> None:
        super().__init__(parent)
        v = QVBoxLayout(self)
        v.setContentsMargins(0, 0, 0, 0)
        v.setSpacing(6)
        lab = QLabel(label)
        lab.setObjectName("fieldLabel")
        self.edit = FileLineEdit()
        self.edit.setPlaceholderText(placeholder)
        self.button = make_button("Выбрать…" if mode == "folder" else "Обзор…")
        row = QHBoxLayout()
        row.setSpacing(8)
        row.addWidget(self.edit, 1)
        row.addWidget(self.button)
        v.addWidget(lab)
        v.addLayout(row)
        self._label, self._mode, self._filter = label, mode, file_filter
        self.button.clicked.connect(self._browse)

    def _browse(self) -> None:
        if self._mode == "folder":
            path = QFileDialog.getExistingDirectory(self, self._label)
        else:
            path, _ = QFileDialog.getOpenFileName(self, self._label, "", self._filter)
        if path:
            self.edit.setText(native(path))

    def text(self) -> str:
        return self.edit.text().strip()


def make_output_box(min_height: int, placeholder: str) -> QPlainTextEdit:
    """Поле для вывода результатов: моноширинный шрифт, без переноса (двоичные строки не ломаются)."""
    box = QPlainTextEdit()
    box.setReadOnly(True)
    box.setFont(QFontDatabase.systemFont(QFontDatabase.FixedFont))
    box.setLineWrapMode(QPlainTextEdit.NoWrap)
    box.setMinimumHeight(min_height)
    box.setPlaceholderText(placeholder)
    return box


def browse_save_file(parent, title: str, file_filter: str, suggested: str = "") -> str:
    path, _ = QFileDialog.getSaveFileName(parent, title, suggested, file_filter)
    return native(path)


def browse_open_file(parent, title: str, file_filter: str = "Все файлы (*)") -> str:
    path, _ = QFileDialog.getOpenFileName(parent, title, "", file_filter)
    return native(path)


def show_warning(parent, text: str) -> None:
    QMessageBox.warning(parent, "Внимание", text)


def show_banner(banner: QLabel, text: str, state: str) -> None:
    """Цветная плашка результата. state: "ok" или "bad"."""
    banner.setText(text)
    banner.setProperty("state", state)
    banner.style().unpolish(banner)
    banner.style().polish(banner)
    banner.show()


def hbox(*items, spacing: int = 10, stretch_after: Optional[int] = None) -> QHBoxLayout:
    """Строка из виджетов; None в списке = растяжка."""
    row = QHBoxLayout()
    row.setSpacing(spacing)
    for it in items:
        if it is None:
            row.addStretch(1)
        else:
            row.addWidget(it)
    return row
