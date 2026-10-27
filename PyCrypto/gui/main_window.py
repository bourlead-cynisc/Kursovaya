from PySide6.QtCore import Qt
from PySide6.QtWidgets import (QButtonGroup, QFrame, QHBoxLayout, QLabel, QMainWindow, QProgressBar,
                               QScrollArea, QSplitter, QStackedWidget, QVBoxLayout, QWidget)

from core.logger import log

from .aes_page import create_aes_page
from .context import AppContext
from .ecdsa_page import create_ecdsa_page
from .kuznyechik_page import create_kuznyechik_page
from .log_panel import LogPanel
from .magma_page import create_magma_page
from .rsa_page import create_rsa_page
from .sha_page import create_sha_page
from .streebog_page import create_streebog_page
from .theme import apply_theme
from .widgets import make_button

# (подпись вкладки, подсказка, фабрика страницы) — порядок как в программе на C++
PAGES = (
    ("AES", "Симметричное шифрование файлов", create_aes_page),
    ("RSA", "Гибридное шифрование (RSA + AES)", create_rsa_page),
    ("SHA-256", "Контрольная сумма файла", create_sha_page),
    ("ECDSA", "Цифровая подпись файла", create_ecdsa_page),
    ("Магма", "ГОСТ Р 34.12-2015, блок 64 бита", create_magma_page),
    ("Стрибог", "ГОСТ Р 34.11-2012, хеш-функция", create_streebog_page),
    ("Кузнечик", "ГОСТ Р 34.12-2015, блок 128 бит", create_kuznyechik_page),
)


class MainWindow(QMainWindow):
    def __init__(self) -> None:
        super().__init__()
        self.setWindowTitle("PyCrypto Studio")
        self.resize(1320, 860)
        self.setMinimumSize(1000, 640)

        ctx = AppContext(self)
        self.ctx = ctx

        root = QWidget()
        root.setObjectName("root")
        outer = QVBoxLayout(root)
        outer.setContentsMargins(0, 0, 0, 0)
        outer.setSpacing(0)

        stack = QStackedWidget()
        for _, _, factory in PAGES:
            stack.addWidget(factory(ctx))

        outer.addWidget(self._build_header(ctx, stack))

        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        scroll.setFrameShape(QFrame.NoFrame)
        scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        holder = QWidget()
        hl = QVBoxLayout(holder)
        hl.setContentsMargins(28, 22, 28, 22)
        hl.addWidget(stack)
        scroll.setWidget(holder)
        ctx.content = scroll

        log_panel = LogPanel()
        split = QSplitter(Qt.Horizontal)
        split.setChildrenCollapsible(False)
        split.setHandleWidth(1)
        split.addWidget(scroll)
        split.addWidget(log_panel)
        split.setStretchFactor(0, 1)
        split.setStretchFactor(1, 0)
        split.setSizes([900, 420])
        outer.addWidget(split, 1)

        outer.addWidget(self._build_footer(ctx))
        self.setCentralWidget(root)

        apply_theme(ctx.dark)
        log("Приложение запущено")

    def _build_header(self, ctx, stack) -> QFrame:
        header = QFrame()
        header.setObjectName("header")
        h = QHBoxLayout(header)
        h.setContentsMargins(24, 12, 24, 12)
        h.setSpacing(6)

        brand = QLabel("Py")
        brand.setObjectName("brandAccent")
        brand2 = QLabel("Crypto Studio")
        brand2.setObjectName("brand")
        h.addWidget(brand)
        h.addWidget(brand2)
        h.addSpacing(28)

        group = QButtonGroup(header)
        group.setExclusive(True)
        for i, (text, tip, _) in enumerate(PAGES):
            b = make_button(text)
            b.setProperty("nav", True)
            b.setCheckable(True)
            b.setChecked(i == 0)
            b.setToolTip(tip)
            b.setMinimumHeight(34)
            group.addButton(b, i)
            h.addWidget(b)
            b.clicked.connect(lambda _=False, idx=i, t=text: (stack.setCurrentIndex(idx), log(f"Выбран режим {t}")))
        h.addStretch(1)

        theme = make_button("☀  Светлая тема", "ghost")

        def toggle() -> None:
            ctx.dark = not ctx.dark
            apply_theme(ctx.dark)
            theme.setText("☀  Светлая тема" if ctx.dark else "☾  Тёмная тема")
        theme.clicked.connect(toggle)
        h.addWidget(theme)
        return header

    def _build_footer(self, ctx) -> QFrame:
        footer = QFrame()
        footer.setObjectName("footer")
        f = QHBoxLayout(footer)
        f.setContentsMargins(24, 8, 24, 8)
        status = QLabel("Готово")
        status.setObjectName("status")
        progress = QProgressBar()
        progress.setTextVisible(False)
        progress.setRange(0, 1)
        progress.setValue(0)
        progress.setFixedWidth(200)
        f.addWidget(status)
        f.addStretch(1)
        f.addWidget(progress)
        ctx.status_label, ctx.progress = status, progress
        return footer
