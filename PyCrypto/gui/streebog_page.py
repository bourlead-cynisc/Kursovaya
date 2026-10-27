from PySide6.QtWidgets import QApplication, QComboBox, QLabel, QVBoxLayout, QWidget

from core.logger import log
from core.streebog import hash_file_streebog

from .tasks import run_task
from .widgets import Card, FileRow, PageHeader, hbox, make_button, make_output_box, show_warning


def create_streebog_page(ctx) -> QWidget:
    page = QWidget()
    root = QVBoxLayout(page)
    root.setContentsMargins(0, 0, 0, 0)
    root.setSpacing(16)
    root.addWidget(PageHeader("Стрибог", "Хеш-функция ГОСТ Р 34.11-2012 (256 или 512 бит), реализована вручную "
                                        "по RFC 6986: матрица A, таблица π и 12 констант. Результат — в двоичном виде."))

    file_card = Card("Файл")
    inp = FileRow("Файл для хеширования", "Выберите или перетащите файл")
    file_card.body.addWidget(inp)
    size_label = QLabel("Длина хеша")
    size_label.setObjectName("fieldLabel")
    combo = QComboBox()
    combo.addItem("256 бит", 256)
    combo.addItem("512 бит", 512)
    combo.setMaximumWidth(220)
    col = QVBoxLayout()
    col.setSpacing(6)
    col.addWidget(size_label)
    col.addWidget(combo)
    compute = make_button("Вычислить хеш", "primary")
    row = hbox(None, compute)
    row.insertLayout(0, col)
    file_card.body.addLayout(row)
    root.addWidget(file_card)

    res = Card("Результат")
    result = make_output_box(280, "Здесь появится хеш…")
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
        path, bits, box = inp.text(), combo.currentData(), {}
        run_task(ctx, f"Стрибог-{bits}", lambda: box.update(text=hash_file_streebog(path, bits)),
                 lambda: result.setPlainText(box["text"]))

    copy.clicked.connect(do_copy)
    compute.clicked.connect(do_compute)
    return page
