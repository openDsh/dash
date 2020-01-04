#include <QApplication>

#include <app/window.hpp>

int main(int argc, char *argv[])
{
    QApplication qApplication(argc, argv);

    DashMainWindow w;
    w.setWindowFlags(Qt::FramelessWindowHint);
    if (std::atoi(std::getenv("DEBUG")))
        w.setFixedSize(1024 * RESOLUTION, 600 * RESOLUTION);
    else {
        qApplication.setOverrideCursor(Qt::BlankCursor);
        w.setFixedSize(qApplication.desktop()->screenGeometry().size());
    }
    w.move(0, 0);
    w.show();

    w.start_open_auto();

    return qApplication.exec();
}
