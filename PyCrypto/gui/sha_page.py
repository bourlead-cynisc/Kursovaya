from PySide6.QtWidgets import QApplication, QVBoxLayout, QWidget

from core.logger import log
from core.sha import hash_file_sha256

from .tasks import run_task
from .widgets import Card, FileRow, PageHeader, hbox, make_button, make_output_box, show_warning


def create_sha_page(ctx) -> QWidget:
    page = QWidget()
    root = QVBoxLayout(page)
    root.setContentsMargins(0, 0, 0, 0)
    root.setSpacing(16)
    root.addWidget(PageHeader("SHA-256", "Контрольная сумма файла (pycryptodome). Файл читается потоком, "
                                         "поэтому размер не ограничен. Результат — в двоичном виде."))

    file_card = Card("Файл")
    inp = FileRow("Файл для хеширования", "Выберите или перетащите файл")
    file_card.body.addWidget(inp)
    compute = make_button("Вычислить SHA-256", "primary")
    file_card.body.addLayout(hbox(None, compute))
    root.addWidget(file_card)

    res = Card("Результат")
    result = make_output_box(200, "Здесь появится хеш…")
    copy = make_button("Копировать")
    res.body.addWidget(result)
    res.body.addLayout(hbox(None, copy))
    root.addWidget(res)
    root.addStretch(1)

    def do_copy() -> None:
        QApplication.clipboard().setText(result.toPlainText())
        log("Результат скопирован в буфер обмена")

    def do_compute() -> None:
        if not inp.text():
            show_warning(page, "Выберите файл.")
            return
        path, box = inp.text(), {}
        run_task(ctx, "SHA-256", lambda: box.update(text=hash_file_sha256(path)),
                 lambda: result.setPlainText(box["text"]))

    copy.clicked.connect(do_copy)
    compute.clicked.connect(do_compute)
    return page
