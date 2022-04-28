#pragma once

#include <QObject>
#include <QTimer>

class Clock : public QObject {
    Q_OBJECT

    public:
    Clock();

    private:
    QTimer timer;

    signals:
    void ticked(QTime time);
};
