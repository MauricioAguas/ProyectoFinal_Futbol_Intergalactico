#include "hds/Game.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    Game ventana;
    ventana.show();

    return app.exec();
}
