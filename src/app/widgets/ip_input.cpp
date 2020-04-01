#include <QColor>
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QList>

#include <app/widgets/ip_input.hpp>

IpInput::IpAddress::IpAddress(QString address_str)
{
    QStringList numbers = address_str.split(".");

    uint32_t a = numbers[0].toInt(), b = numbers[1].toInt(), c = numbers[2].toInt(), d = numbers[3].toInt();

    this->address = (((((a << 8) + b) << 8) + c) << 8) + d;
}

QStringList IpInput::IpAddress::to_str_list(bool pad)
{
    QStringList numbers;

    int padding = pad ? 3 : 0;
    numbers.append(QString::number((this->address & 0xff000000) >> 24).rightJustified(padding, '0'));
    numbers.append(QString::number((this->address & 0x00ff0000) >> 16).rightJustified(padding, '0'));
    numbers.append(QString::number((this->address & 0x0000ff00) >> 8).rightJustified(padding, '0'));
    numbers.append(QString::number((this->address & 0x000000ff)).rightJustified(padding, '0'));

    return numbers;
}

IpInput::IpInput(QStringList addresses, QFont font, QWidget *parent) : QWidget(parent), last_saved_address(DEFAULT_IP)
{
    this->theme = Theme::get_instance();
    this->reset_timer = new QElapsedTimer();
    this->font = font;
    this->update_addresses(addresses);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QPushButton *up_button = new QPushButton(this);
    up_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    up_button->setFlat(true);
    up_button->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("arrow_drop_up", up_button);
    connect(up_button, &QPushButton::clicked, [this]() {
        int count = this->addresses.size();
        if (count > 0) this->update(this->addresses[(this->addresses.indexOf(this->last_saved_address) + 1) % count]);
    });

    QPushButton *down_button = new QPushButton(this);
    down_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    down_button->setFlat(true);
    down_button->setIconSize(Theme::icon_32);
    this->theme->add_button_icon("arrow_drop_down", down_button);
    connect(down_button, &QPushButton::clicked, [this]() {
        int count = this->addresses.size();
        if (count > 0) {
            int idx = ((this->addresses.indexOf(this->last_saved_address) - 1) % count + count) % count;
            this->update(this->addresses[idx]);
        }
    });

    layout->addWidget(up_button, 0, Qt::AlignCenter);
    layout->addWidget(this->input_widget());
    layout->addWidget(down_button, 0, Qt::AlignCenter);
}

QWidget *IpInput::input_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->addStretch();

    QList<QPushButton *> numbers;
    for (auto numbers : this->last_saved_address.to_str_list(true)) {
        for (auto number : numbers) {
            QPushButton *button = new QPushButton(QString(number), widget);
            button->setFont(this->font);
            button->setFlat(true);
            this->inputs.append(button);
            connect(button, &QPushButton::pressed, [this]() { this->reset_timer->start(); });
            connect(button, &QPushButton::released, [this, button]() {
                button->setText(
                    this->reset_timer->hasExpired(500) ? "0" : QString::number((button->text().toInt() + 1) % 10));
            });
            layout->addWidget(button);
        }
    }
    layout->addStretch();

    for (int i = 0; i < 3; i++) {
        QLabel *dot = new QLabel(".", widget);
        dot->setFont(this->font);
        layout->insertWidget(i + ((i * 3) + 4), dot);
    }

    return widget;
}

QString IpInput::active_address()
{
    QString address;

    for (int i = 0; i < 12; i += 3) {
        if (i > 0) address.append('.');
        address.append(QString::number((this->inputs[i]->text().toInt() * 100) +
                                       (this->inputs[i + 1]->text().toInt() * 10) +
                                       this->inputs[i + 2]->text().toInt()));
    }

    return address;
}

void IpInput::update(IpAddress address)
{
    int idx = 0;
    for (auto numbers : address.to_str_list(true))
        for (auto number : numbers) this->inputs[idx++]->setText(QString(number));

    this->last_saved_address = address;
}
