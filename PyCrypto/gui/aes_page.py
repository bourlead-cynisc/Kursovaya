import os

from PySide6.QtWidgets import QComboBox, QLabel, QVBoxLayout, QWidget

from core.aes import aes_decrypt, aes_encrypt, aes_generate_key
from core.logger import log

from .tasks import run_task
from .widgets import (Card, FileRow, PageHeader, browse_save_file, hbox, make_button, native,
                      show_warning)


def create_aes_page(ctx) -> QWidget:
    page = QWidget()
    root = QVBoxLayout(page)
    root.setContentsMargins(0, 0, 0, 0)
    root.setSpacing(16)
    root.addWidget(PageHeader("AES", "Симметричное шифрование файлов: AES-CBC с PKCS#7 и случайным IV "
                                     "(библиотека pycryptodome)."))

    files = Card("Файлы", "Выберите файл кнопкой или перетащите его в поле.")
    inp = FileRow("Входной файл", "Что шифруем / расшифровываем")
    out = FileRow("Папка результата", "Куда сохранить результат", "folder")
    key = FileRow("Файл ключа", "*.key (создайте кнопкой «Сгенерировать ключ»)", "file",
                  "Ключи (*.key);;Все файлы (*)")
    for w in (inp, out, key):
        files.body.addWidget(w)
    root.addWidget(files)

    params = Card("Параметры")
    size_label = QLabel("Размер ключа")
    size_label.setObjectName("fieldLabel")
    combo = QComboBox()
    for text, bits in (("128 бит", 128), ("192 бита", 192), ("256 бит", 256)):
        combo.addItem(text, bits)
    combo.setCurrentIndex(2)
    combo.setMaximumWidth(220)
    col = QVBoxLayout()
    col.setSpacing(6)
    col.addWidget(size_label)
    col.addWidget(combo)
    gen_btn = make_button("Сгенерировать ключ")
    dec_btn = make_button("Расшифровать")
    enc_btn = make_button("Зашифровать", "primary")
    actions = hbox(None, gen_btn, dec_btn, enc_btn)
    actions.insertLayout(0, col)
    params.body.addLayout(actions)
    root.addWidget(params)
    root.addStretch(1)

    # Удобства: папка результата = папка входного файла; размер ключа определяется по длине файла ключа.
    def on_input(text: str) -> None:
        if not out.text() and os.path.isfile(text):
            out.edit.setText(native(os.path.dirname(os.path.abspath(text))))

    def on_key(text: str) -> None:
        size = os.path.getsize(text) if os.path.isfile(text) else -1
        if size in (16, 24, 32):
            combo.setCurrentIndex({16: 0, 24: 1, 32: 2}[size])

    inp.edit.textChanged.connect(on_input)
    key.edit.textChanged.connect(on_key)

    def generate() -> None:
        bits = combo.currentData()
        path = browse_save_file(page, "Сохранить ключ", "Ключи (*.key)", f"aes_{bits}.key")
        if not path:
            return

        def done() -> None:
            key.edit.setText(path)
            log(f"Ключ сохранён: {path}")
        run_task(ctx, f"Генерация ключа AES-{bits}", lambda: aes_generate_key(bits, path), done)

    def validate() -> bool:
        if not (inp.text() and out.text() and key.text()):
            show_warning(page, "Заполните все поля: входной файл, папка результата и ключ.")
            return False
        return True

    def operation(encrypt: bool) -> None:
        if not validate():
            return
        bits, i, o, k = combo.currentData(), inp.text(), out.text(), key.text()
        result = {}
        fn = aes_encrypt if encrypt else aes_decrypt
        what = "шифрование" if encrypt else "расшифровка"
        run_task(ctx, f"AES-{bits}: {what}", lambda: result.update(path=fn(i, o, k, bits)),
                 lambda: log(f"Результат: {native(str(result['path']))}"))

    gen_btn.clicked.connect(generate)
    enc_btn.clicked.connect(lambda: operation(True))
    dec_btn.clicked.connect(lambda: operation(False))
    return page
