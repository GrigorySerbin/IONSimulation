#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //QApplication::setOrganizationName("IONSimulator");
    //QApplication::setApplicationName("IONSimulator");

    MainWindow w;
    w.show();

    return a.exec();
}
