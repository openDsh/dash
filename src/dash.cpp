#include <QApplication>
#include <QStringList>
#include <QWindow>

#include "app/window.hpp"

int main(int argc, char *argv[])
{
    QApplication dash(argc, argv);
    QSplashScreen splash;

    dash.setOrganizationName("openDsh");
    dash.setApplicationName("dash");

    QStringList args = dash.arguments();
    bool use_fixed_size = (args.size() > 2);

    QSize size = dash.primaryScreen()->size();
    if (use_fixed_size)
        size = QSize(args.at(1).toInt(), args.at(2).toInt());

    splash.setPixmap(QPixmap(":/splash.png").scaled(size, Qt::KeepAspectRatio));
    splash.show();
    dash.processEvents();

    DashWindow window;
    window.setWindowIcon(QIcon(":/logo.png"));
    window.setWindowFlags(Qt::FramelessWindowHint);
    if (!use_fixed_size)
        window.setWindowState(Qt::WindowFullScreen);

    // force to either screen or custom size
    window.setFixedSize(size);
    window.show();
    splash.finish(&window);

    return dash.exec();
}
