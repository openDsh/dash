#pragma once

#include <QWidget>
#include <QLabel>
#include <QSlider>

#include "app/widgets/dialog.hpp"
#include "app/theme.hpp"

class ColorPicker : public QWidget {
    Q_OBJECT

   public:
    ColorPicker(QSize block_size, QFont font, QWidget *parent = nullptr);

    void update(QColor color);

   private:
    QSlider *r_slider;
    QSlider *g_slider;
    QSlider *b_slider;

    QFont font;
    QFrame *color_hint;
    QPushButton *button;
    QPalette hint_palette;

    double scale;

    inline void set_icon()
    {
        QPixmap block(this->button->iconSize());
        block.fill(this->color());
        this->button->setIcon(block);
    }
    inline QLayout *component_slider(QSlider *slider)
    {
        QHBoxLayout *layout = new QHBoxLayout();
        layout->setContentsMargins(0, 0, 0, 0);

        slider->setProperty("neutral", true);
        slider->setTracking(false);
        slider->setRange(0, 255);
        slider->setFixedHeight(30 * this->scale);

        QLabel *label = new QLabel(QString::number(slider->value()));
        label->setFont(this->font);
        label->setIndent(8);
        QObject::connect(slider, &QSlider::valueChanged, [this, label](int position) {
            label->setText(QString::number(position));
            this->hint_palette.setColor(QPalette::Window, this->color());
            this->color_hint->setPalette(this->hint_palette);
        });

        layout->addWidget(slider, 4);
        layout->addWidget(label, 2);

        return layout;
    }
    inline QColor color() { return QColor(this->r_slider->value(), this->g_slider->value(), this->b_slider->value()); }

    QWidget *dialog_body();

   signals:
    void color_changed(QColor color);
};
