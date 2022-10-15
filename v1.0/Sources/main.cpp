#include "mainwindow.h"
#include "board.h"

const int s_size = 10;
int argc;
char* argv[s_size];

QApplication a(argc, argv);
MainWindow win;

int main(int argc, char *argv[])
{
    win.show();
    return a.exec();
}
