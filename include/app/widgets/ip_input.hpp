#ifndef IP_INPUT_HPP_
#define IP_INPUT_HPP_

#include <QFont>
#include <QList>
#include <QPushButton>
#include <QWidget>
#include <QElapsedTimer>
#include <array>

#include <app/theme.hpp>

class IpInput : public QWidget {
    Q_OBJECT

   public:
    IpInput(QStringList addresses, QFont font, QWidget *parent = nullptr);
    QString active_address();
    inline void set_last_saved_address(QString address) { this->update(IpAddress(address)); }
    inline void update_addresses(QStringList addresses)
    {
        this->addresses.clear();
        for (auto address : addresses) this->addresses.append(IpAddress(address));
    }

   private:
    struct IpAddress {
        IpAddress(QString address);
        QStringList to_str_list(bool pad = false);
        inline QString to_str(bool pad) { return this->to_str_list(pad).join("."); }

        uint32_t address;

        bool operator==(const IpAddress &rhs) const { return this->address == rhs.address; }
    };

    QWidget *input_widget();
    void update(IpAddress address);

    const IpAddress DEFAULT_IP = IpAddress("0.0.0.0");

    QList<QPushButton *> inputs;
    QList<IpAddress> addresses;
    IpAddress last_saved_address;
    QFont font;
    QElapsedTimer *reset_timer;
    Theme *theme;
};

#endif
