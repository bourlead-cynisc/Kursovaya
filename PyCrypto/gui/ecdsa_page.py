from PySide6.QtWidgets import QComboBox, QLabel, QVBoxLayout, QWidget

from core.ecdsa import CURVES, ecdsa_generate_keys, ecdsa_public_key_path_for, ecdsa_sign, ecdsa_verify
from core.logger import log

from .tasks import run_task
from .widgets import (Card, FileRow, PageHeader, browse_open_file, browse_save_file, hbox, make_button,
                      native, show_banner, show_warning)


def create_ecdsa_page(ctx) -> QWidget:
    page = QWidget()
    root = QVBoxLayout(page)
    root.setContentsMargins(0, 0, 0, 0)
    root.setSpacing(16)
    root.addWidget(PageHeader("ECDSA", "Цифровая подпись файла на эллиптических кривых (pycryptodome): "
                                      "подписывается SHA-256 файла."))

    key_filter = "Ключи (*.key);;Все файлы (*)"
    files = Card("Файлы и ключи")
    inp = FileRow("Файл", "Что подписываем / проверяем")
    priv = FileRow("Закрытый ключ (для подписи)", "*.key", "file", key_filter)
    pub = FileRow("Открытый ключ (для проверки)", "*_pub.key", "file", key_filter)
    for w in (inp, priv, pub):
        files.body.addWidget(w)
    root.addWidget(files)

    params = Card("Действия")
    curve_label = QLabel("Кривая (для генерации ключей)")
    curve_label.setObjectName("fieldLabel")
    combo = QComboBox()
    for c in CURVES:
        combo.addItem(c, c)
    combo.setMaximumWidth(220)
    col = QVBoxLayout()
    col.setSpacing(6)
    col.addWidget(curve_label)
    col.addWidget(combo)
    gen_btn = make_button("Сгенерировать ключи")
    verify_btn = make_button("Проверить подпись")
    sign_btn = make_button("Подписать", "primary")
    actions = hbox(None, gen_btn, verify_btn, sign_btn)
    actions.insertLayout(0, col)
    params.body.addLayout(actions)
    root.addWidget(params)

    banner = QLabel()
    banner.setObjectName("banner")
    banner.hide()
    root.addWidget(banner)
    root.addStretch(1)

    inp.edit.textChanged.connect(lambda _: banner.hide())

    def generate() -> None:
        curve = combo.currentData()
        priv_path = browse_save_file(page, "Сохранить закрытый ключ", "Ключи (*.key)", "ecdsa_private.key")
        if not priv_path:
            return
        pub_path = str(ecdsa_public_key_path_for(priv_path))

        def done() -> None:
            priv.edit.setText(priv_path)
            pub.edit.setText(native(pub_path))
            log(f"Закрытый ключ: {priv_path}")
            log(f"Открытый ключ: {native(pub_path)}")
        run_task(ctx, f"Генерация ECDSA ({curve})", lambda: ecdsa_generate_keys(curve, priv_path, pub_path), done)

    def sign() -> None:
        if not (inp.text() and priv.text()):
            show_warning(page, "Выберите файл и закрытый ключ.")
            return
        sig_path = browse_save_file(page, "Сохранить подпись", "Подписи (*.sig)", "signature.sig")
        if not sig_path:
            return
        i, k = inp.text(), priv.text()

        def done() -> None:
            log(f"Подпись сохранена: {sig_path}")
            show_banner(banner, "Подпись создана и сохранена", "ok")
        run_task(ctx, "ECDSA: подпись", lambda: ecdsa_sign(i, k, sig_path), done)

    def verify() -> None:
        if not (inp.text() and pub.text()):
            show_warning(page, "Выберите файл и открытый ключ.")
            return
        sig_path = browse_open_file(page, "Выберите файл подписи", "Подписи (*.sig);;Все файлы (*)")
        if not sig_path:
            return
        i, k, box = inp.text(), pub.text(), {}

        def done() -> None:
            if box["ok"]:
                show_banner(banner, "✔  Подпись верна — файл не изменялся", "ok")
            else:
                show_banner(banner, "✖  Подпись НЕВЕРНА — файл изменён или ключ не тот", "bad")
        run_task(ctx, "ECDSA: проверка", lambda: box.update(ok=ecdsa_verify(i, k, sig_path)), done)

    gen_btn.clicked.connect(generate)
    sign_btn.clicked.connect(sign)
    verify_btn.clicked.connect(verify)
    return page
