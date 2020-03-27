#include <QColor>
#include <QHBoxLayout>
#include <QMap>
#include <QString>

#include <app/config.hpp>
#include <app/widgets/color_label.hpp>

ColorLabel::ColorLabel(QSize block_size, QWidget *parent) : QWidget(parent)
{
    this->theme = Theme::get_instance();

    QString default_color = Config::get_instance()->get_color();

    QHBoxLayout *layout = new QHBoxLayout(this);

    this->icon = new QLabel(this);
    QPixmap block(block_size);
    block.fill(this->theme->get_color(default_color));
    this->icon->setPixmap(block);

    this->name = new QLabel(this);
    this->name->setText(default_color);

    connect(this->theme, &Theme::color_updated, [this]() { this->update(this->text()); });

    layout->addStretch();
    layout->addWidget(this->icon);
    layout->addWidget(this->name);
    layout->addStretch();
}

void ColorLabel::update(QString color)
{
    this->name->setText(color);

    QPixmap block(this->icon->pixmap()->size());
    block.fill(this->theme->get_color(color));
    this->icon->setPixmap(block);
}