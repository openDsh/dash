#include <QApplication>
#include <QStringList>
#include <QWindow>

#include "app/window.hpp"

int main(int argc, char *argv[])
{
    QApplication dash(argc, argv);
    QStringList args = QCoreApplication::arguments();

    DashWindow window;
    window.setWindowIcon(QIcon(":/logo.png"));
    window.setWindowFlags(Qt::FramelessWindowHint);
    if (args.size() > 2)
        window.setFixedSize(args.at(1).toInt(), args.at(2).toInt());
    else
        window.setWindowState(Qt::WindowFullScreen);

    window.show();

    return dash.exec();
}
