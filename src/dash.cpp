#include <QApplication>
#include <StringList>
#include <QWindow>

#include "app/window.hpp"

int main(int argc, char *argv[])
{
    QApplication dash(argc, argv);
    QPixmap pixmap(":/splash.png");
    QSplashScreen *splash = new QSplashScreen(pixmap);
    splash.show();
    splash->showFullScreen();
    QStringList args = dash.arguments();

    DashWindow window;
    window.setWindowIcon(QIcon(":/logo.png"));
    window.setWindowFlags(Qt::FramelessWindowHint);

    QSize size = dash.primaryScreen()->size();
    if (args.size() > 2)
        size = QSize(args.at(1).toInt(), args.at(2).toInt());
    else
        window.setWindowState(Qt::WindowFullScreen);

    // force to either screen or custom size
    window.setFixedSize(size);
    window.show();
    delete splash;

    return dash.exec();
}
