#pragma once

#include <QFont>
#include <QList>
#include <QPushButton>
#include <QWidget>
#include <QElapsedTimer>
#include <array>

#include "app/theme.hpp"

class IpInput : public QWidget {
    Q_OBJECT

   public:
    IpInput(QString address, QFont font, QWidget *parent = nullptr);
    QString active_address();

   private:
    struct IpAddress {
        IpAddress(QString address);
        QStringList to_str_list(bool pad = false);
        inline QString to_str(bool pad) { return this->to_str_list(pad).join("."); }

        uint32_t address;

        bool operator==(const IpAddress &rhs) const { return this->address == rhs.address; }
    };

    QWidget *input_widget(IpAddress address);

    QList<QPushButton *> inputs;
    QFont font;
    QElapsedTimer *reset_timer;
    Theme *theme;
};
