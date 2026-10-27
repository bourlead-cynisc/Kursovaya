from PySide6.QtCore import QObject


class AppContext(QObject):
    """Общее состояние приложения (окно, индикаторы, флаг занятости)."""

    def __init__(self, window) -> None:
        super().__init__(window)
        self.window = window
        self.content = None        # блокируется на время операции
        self.progress = None
        self.status_label = None
        self.busy = False
        self.dark = True
