#include "mainwindow.h"

#include <QApplication>

#include "mocp/client.c"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
