#include "mainwindow.h"
#include "apiwindow.h"
#include <QApplication>
#define API_MODE 1

int main(int argc, char *argv[])
{
#if API_MODE
    QApplication a(argc, argv);

    APIWindow w;
    w.show();

    return a.exec();
#else
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
#endif
}
