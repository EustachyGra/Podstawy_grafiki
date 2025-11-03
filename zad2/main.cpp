#include "mainwindow.h"
#include "ekran.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    Ekran e;
    e.show();
    e.setGeometry(100,100,800,400);
   // w.show();
    return a.exec();
}
