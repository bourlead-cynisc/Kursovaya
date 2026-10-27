import os

from PySide6.QtWidgets import QComboBox, QLabel, QVBoxLayout, QWidget

from core.files import file_exists
from core.logger import log
from core.rsa import rsa_decrypt, rsa_encrypt, rsa_generate_keys

from .tasks import run_task
from .widgets import Card, FileRow, PageHeader, hbox, make_button, native, show_warning


def create_rsa_page(ctx) -> QWidget:
    page = QWidget()
    root = QVBoxLayout(page)
    root.setContentsMargins(0, 0, 0, 0)
    root.setSpacing(16)
    root.addWidget(PageHeader("RSA", "Гибридное шифрование: файл шифруется AES-256, а ключ AES — RSA-OAEP "
                                     "(pycryptodome)."))

    files = Card("Файлы", "В папке ключей используются файлы private_<размер>.key и public_<размер>.key.")
    inp = FileRow("Входной файл", "Что шифруем / расшифровываем")
    out = FileRow("Папка результата", "Куда сохранить результат", "folder")
    keys = FileRow("Папка с ключами", "Где лежат (или будут созданы) ключи", "folder")
    for w in (inp, out, keys):
        files.body.addWidget(w)
    root.addWidget(files)

    params = Card("Параметры")
    size_label = QLabel("Размер ключа")
    size_label.setObjectName("fieldLabel")
    combo = QComboBox()
    for bits in (1024, 2048, 4096, 8192, 16384):
        combo.addItem(f"{bits} бит", bits)
    combo.setCurrentIndex(1)
    combo.setMaximumWidth(220)
    hint = QLabel("Генерация ключей 8192 бит и больше может занять несколько минут.")
    hint.setObjectName("muted")
    hint.setWordWrap(True)
    hint.setVisible(False)
    combo.currentIndexChanged.connect(lambda _: hint.setVisible(combo.currentData() >= 8192))

    col = QVBoxLayout()
    col.setSpacing(6)
    col.addWidget(size_label)
    col.addWidget(combo)
    gen_btn = make_button("Сгенерировать ключи")
    dec_btn = make_button("Расшифровать")
    enc_btn = make_button("Зашифровать", "primary")
    actions = hbox(None, gen_btn, dec_btn, enc_btn)
    actions.insertLayout(0, col)
    params.body.addLayout(actions)
    params.body.addWidget(hint)
    root.addWidget(params)
    root.addStretch(1)

    inp.edit.textChanged.connect(lambda t: out.edit.setText(native(os.path.dirname(os.path.abspath(t))))
                                 if not out.text() and os.path.isfile(t) else None)

    def generate() -> None:
        if not keys.text():
            show_warning(page, "Сначала выберите папку с ключами.")
            return
        bits, d = combo.currentData(), keys.text()
        run_task(ctx, f"Генерация RSA-{bits}", lambda: rsa_generate_keys(bits, d),
                 lambda: log(f"Ключи сохранены в: {d}"))

    def operation(encrypt: bool) -> None:
        if not (inp.text() and out.text() and keys.text()):
            show_warning(page, "Заполните все поля: входной файл, папка результата и папка ключей.")
            return
        bits = combo.currentData()
        name = f"public_{bits}.key" if encrypt else f"private_{bits}.key"
        key_path = os.path.join(keys.text(), name)
        if not file_exists(key_path):
            show_warning(page, ("Открытый" if encrypt else "Закрытый") + " ключ не найден. Сначала сгенерируйте ключи.")
            return
        i, o = inp.text(), out.text()
        result = {}
        fn = rsa_encrypt if encrypt else rsa_decrypt
        what = "шифрование" if encrypt else "расшифровка"
        run_task(ctx, f"RSA-{bits}: {what}", lambda: result.update(path=fn(i, o, key_path, bits)),
                 lambda: log(f"Результат: {native(str(result['path']))}"))

    gen_btn.clicked.connect(generate)
    enc_btn.clicked.connect(lambda: operation(True))
    dec_btn.clicked.connect(lambda: operation(False))
    return page
