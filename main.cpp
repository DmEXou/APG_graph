#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Задача о кратчайшем пути");
    w.setFixedSize(850, 610);
    w.show();
    return a.exec();
}
