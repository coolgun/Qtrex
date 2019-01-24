#include <QtWidgets/QApplication>
#include "mainwnd.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWnd w;
    w.show();
    return a.exec();
}
