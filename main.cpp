#include "chart.h"
#include "mainwindow.h"

#include <QApplication>


using namespace std;


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow mw;
    mw.show();

    return a.exec();
}
