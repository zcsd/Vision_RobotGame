#include <QApplication>
#include "tuner.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    tuner w;
    w.show();
    
    return a.exec();
}
