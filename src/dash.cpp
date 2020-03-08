#include <QApplication>
#include <QStringList>

#include <app/window.hpp>

int main(int argc, char *argv[])
{
    QApplication dash(argc, argv);
    QStringList args = QCoreApplication::arguments();

    MainWindow window;
    window.setWindowFlags(Qt::FramelessWindowHint);
    if (args.size() > 2) {
        int w = args.at(1).toInt();
        int h = args.at(2).toInt();
        window.setFixedSize(w, h);
    }
    else {
        dash.setOverrideCursor(Qt::BlankCursor);
        window.setFixedSize(dash.desktop()->screenGeometry().size());
    }
    window.move(0, 0);
    window.show();

    return dash.exec();
}
