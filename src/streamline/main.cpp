#include <QDebug>
#include <QApplication>

#include <defines.h>
#include <clw.h>
#include "application.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Application* window = new Application();
    window->resize(FRAME_WIDTH, FRAME_HEIGHT);
    window->show();
    return app.exec();
}
