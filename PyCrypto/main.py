"""PyCrypto Studio — графическая крипто-утилита: AES, RSA, SHA-256, ECDSA (pycryptodome)
и ГОСТ: Магма, Стрибог, Кузнечик (реализованы вручную)."""
import sys

from PySide6.QtWidgets import QApplication, QStyleFactory

from gui.main_window import MainWindow


def main() -> int:
    app = QApplication(sys.argv)
    app.setApplicationName("PyCrypto Studio")
    app.setStyle(QStyleFactory.create("Fusion"))
    window = MainWindow()
    window.show()
    return app.exec()


if __name__ == "__main__":
    sys.exit(main())
