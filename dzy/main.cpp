#include "dzy.h"
#include "zhu.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    SetConsoleOutputCP(65001);
    QApplication a(argc, argv);
    zhu w;
    w.show();
    return a.exec();
}
