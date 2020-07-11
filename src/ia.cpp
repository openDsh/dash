#include <QApplication>
#include <QStringList>

#include "app/window.hpp"

int main(int argc, char *argv[])
{
    QApplication ia(argc, argv);
    QStringList args = QCoreApplication::arguments();

    MainWindow window;
    window.setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    if (args.size() > 2)
        window.setFixedSize(args.at(1).toInt(), args.at(2).toInt());
    else
        window.setFixedSize(ia.desktop()->screenGeometry().size());
    window.move(0, 0);
    window.show();

    return ia.exec();
}
