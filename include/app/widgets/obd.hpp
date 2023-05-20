#pragma once

#include <QString>
#include <QWidget>

class Arbiter;

class Obd : public QWidget {
    Q_OBJECT

   public:
    Obd(Arbiter &arbiter, QWidget *parent = nullptr);

   private:
};
