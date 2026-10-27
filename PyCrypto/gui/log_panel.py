"""Журнал справа в виде тёмной консоли. Принимает сообщения из любых потоков."""
from PySide6.QtCore import Signal, Slot
from PySide6.QtWidgets import QFrame, QHBoxLayout, QLabel, QPlainTextEdit, QVBoxLayout

from core.logger import log, set_log_sink

from .widgets import make_button


class LogPanel(QFrame):
    _message = Signal(str)

    def __init__(self, parent=None) -> None:
        super().__init__(parent)
        self.setObjectName("logPanel")
        v = QVBoxLayout(self)
        v.setContentsMargins(16, 14, 16, 14)
        v.setSpacing(10)

        head = QHBoxLayout()
        title = QLabel("ЖУРНАЛ")
        title.setObjectName("logTitle")
        clear = make_button("Очистить")
        clear.setObjectName("logClear")
        clear.setMinimumHeight(28)
        head.addWidget(title)
        head.addStretch(1)
        head.addWidget(clear)

        self.view = QPlainTextEdit()
        self.view.setObjectName("console")
        self.view.setReadOnly(True)
        self.view.setLineWrapMode(QPlainTextEdit.NoWrap)
        self.view.setMaximumBlockCount(5000)
        self.view.setPlaceholderText("Здесь появятся сообщения о ходе операций…")
        v.addLayout(head)
        v.addWidget(self.view, 1)

        clear.clicked.connect(self._clear)
        self._message.connect(self._append)            # из другого потока Qt доставит сообщение через очередь
        set_log_sink(self._message.emit)

    @Slot(str)
    def _append(self, text: str) -> None:
        self.view.appendPlainText(text)

    def _clear(self) -> None:
        self.view.clear()
        log("Журнал очищен")
