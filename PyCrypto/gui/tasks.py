"""Запуск тяжёлых операций в фоновом потоке, чтобы окно не зависало (например, RSA-16384)."""
from typing import Callable, Optional

from PySide6.QtCore import QObject, QRunnable, QThreadPool, Signal, Slot
from PySide6.QtWidgets import QMessageBox

from core.logger import log


class _Signals(QObject):
    done = Signal(object)          # None при успехе или текст ошибки


class _Worker(QRunnable):
    def __init__(self, work: Callable[[], None], signals: _Signals) -> None:
        super().__init__()
        self._work = work
        self.signals = signals

    def run(self) -> None:
        try:
            self._work()
            self.signals.done.emit(None)
        except Exception as e:                      # noqa: BLE001 — любая ошибка должна дойти до пользователя
            self.signals.done.emit(str(e) or e.__class__.__name__)


class _Handle(QObject):
    """Живёт в GUI-потоке: слот гарантированно выполняется там (очередь событий Qt)."""

    def __init__(self, ctx, title: str, on_success: Optional[Callable[[], None]]) -> None:
        super().__init__(ctx)
        self.ctx, self.title, self.on_success = ctx, title, on_success
        self.signals = _Signals()
        self.signals.done.connect(self.on_done)

    @Slot(object)
    def on_done(self, err) -> None:
        set_busy(self.ctx, False)
        if err:
            log(f"Ошибка: {err}")
            QMessageBox.critical(self.ctx.window, self.title, str(err))
        elif self.on_success:
            self.on_success()
        self.deleteLater()


def set_busy(ctx, busy: bool, message: str = "") -> None:
    ctx.busy = busy
    if ctx.content is not None:
        ctx.content.setEnabled(not busy)
    if ctx.progress is not None:
        ctx.progress.setRange(0, 0 if busy else 1)       # 0..0 = бегущая полоса
        ctx.progress.setValue(0)
    if ctx.status_label is not None:
        ctx.status_label.setText(message if busy else "Готово")


def run_task(ctx, title: str, work: Callable[[], None], on_success: Optional[Callable[[], None]] = None) -> None:
    """work() выполняется в потоке пула; on_success() — потом, в GUI-потоке."""
    if ctx.busy:
        return
    set_busy(ctx, True, title + "…")
    handle = _Handle(ctx, title, on_success)
    QThreadPool.globalInstance().start(_Worker(work, handle.signals))
