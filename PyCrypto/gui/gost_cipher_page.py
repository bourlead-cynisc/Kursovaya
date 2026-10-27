"""Общая страница для Магмы и Кузнечика (ключ 256 бит, CBC; данные показываются в двоичном виде)."""
import os
from dataclasses import dataclass
from typing import Callable

from PySide6.QtWidgets import QVBoxLayout, QWidget

from core.formatting import format_file_size, to_binary_section
from core.logger import log
from core.models import CipherRunInfo

from .tasks import run_task
from .widgets import (Card, FileRow, PageHeader, browse_save_file, hbox, make_button, make_output_box,
                      native, show_warning)


@dataclass
class GostCipherPageConfig:
    title: str
    subtitle: str
    default_key_name: str
    generate_key: Callable[[str], bytes]
    encrypt: Callable[[str, str, str], CipherRunInfo]
    decrypt: Callable[[str, str, str], CipherRunInfo]


def create_gost_cipher_page(ctx, cfg: GostCipherPageConfig) -> QWidget:
    page = QWidget()
    root = QVBoxLayout(page)
    root.setContentsMargins(0, 0, 0, 0)
    root.setSpacing(16)
    root.addWidget(PageHeader(cfg.title, cfg.subtitle))

    files = Card("Файлы и ключ")
    inp = FileRow("Входной файл", "Что шифруем / расшифровываем")
    out = FileRow("Папка результата", "Куда сохранить результат", "folder")
    key = FileRow("Файл ключа", "*.key (создайте кнопкой «Сгенерировать ключ»)", "file",
                  "Ключи (*.key);;Все файлы (*)")
    for w in (inp, out, key):
        files.body.addWidget(w)
    gen_btn = make_button("Сгенерировать ключ")
    dec_btn = make_button("Расшифровать")
    enc_btn = make_button("Зашифровать", "primary")
    files.body.addLayout(hbox(gen_btn, None, dec_btn, enc_btn))
    root.addWidget(files)

    res = Card("Данные в двоичном виде")
    result = make_output_box(250, "Здесь появятся ключ, вектор инициализации и начало шифртекста…")
    res.body.addWidget(result)
    root.addWidget(res)
    root.addStretch(1)

    inp.edit.textChanged.connect(lambda t: out.edit.setText(native(os.path.dirname(os.path.abspath(t))))
                                 if not out.text() and os.path.isfile(t) else None)

    def generate() -> None:
        path = browse_save_file(page, "Сохранить ключ", "Ключи (*.key)", cfg.default_key_name)
        if not path:
            return
        box = {}

        def done() -> None:
            key.edit.setText(path)
            log(f"Ключ сохранён: {path}")
            result.setPlainText("Сгенерирован новый ключ\n\n" + to_binary_section("Ключ", box["key"]))
        run_task(ctx, f"{cfg.title}: генерация ключа", lambda: box.update(key=cfg.generate_key(path)), done)

    def operation(encrypt: bool) -> None:
        if not (inp.text() and out.text() and key.text()):
            show_warning(page, "Заполните все поля: входной файл, папка результата и ключ.")
            return
        i, o, k, box = inp.text(), out.text(), key.text(), {}
        fn = cfg.encrypt if encrypt else cfg.decrypt
        what = "шифрование" if encrypt else "расшифровка"

        def done() -> None:
            info: CipherRunInfo = box["info"]
            log(f"Результат: {native(str(info.output_file))}")
            text = (f"Операция: {what}\nРезультат: {info.output_file.name}\n"
                    f"Размер: {format_file_size(info.input_size)} → {format_file_size(info.output_size)}\n\n"
                    + to_binary_section("Вектор инициализации (IV)", info.iv) + "\n"
                    + to_binary_section("Первые байты шифртекста" if encrypt else "Первые байты открытого текста",
                                        info.preview))
            result.setPlainText(text)
        run_task(ctx, f"{cfg.title}: {what}", lambda: box.update(info=fn(i, o, k)), done)

    gen_btn.clicked.connect(generate)
    enc_btn.clicked.connect(lambda: operation(True))
    dec_btn.clicked.connect(lambda: operation(False))
    return page
