#include <QDebug>

#include "test.hpp"

void Test::init(QString message)
{
    qDebug() << message;
}
