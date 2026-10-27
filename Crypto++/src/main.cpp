// Crypto++ Utility — графическая крипто-утилита на Qt 6.
#include <QApplication>
#include <QStyleFactory>
#include "gui/createMainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Crypto++ Utility"));
    app.setStyle(QStyleFactory::create(QStringLiteral("Fusion")));

    QMainWindow* window = createMainWindow();
    window->show();
    return app.exec();
}
