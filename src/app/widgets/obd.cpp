#include <QHBoxLayout>

#include "app/arbiter.hpp"

#include "app/widgets/obd.hpp"

Obd::Obd(Arbiter &arbiter, QWidget *parent) : QWidget(parent)
{
    this->setObjectName("OBD");

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    //layout->addWidget(this->Obd, 0, Qt::AlignHCenter);

}
