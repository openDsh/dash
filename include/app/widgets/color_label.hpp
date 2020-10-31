#pragma once

#include <QWidget>
#include <QLabel>
#include <QSlider>

#include "app/widgets/dialog.hpp"
#include "app/theme.hpp"

class ColorLabel : public QWidget {
    Q_OBJECT

   public:
    ColorLabel(QSize block_size, QFont font, QWidget *parent = nullptr);

    void scale(double scale);
    void update(QColor color);

   protected:
    void mouseReleaseEvent(QMouseEvent *) override { this->dialog->open(); }

   private:
    QSlider *r_slider;
    QSlider *g_slider;
    QSlider *b_slider;

    Dialog *dialog;

    QSize block_size;
    QFont font;
    QLabel *icon;
    QFrame *color_hint;
    QLabel *name;
    QPalette hint_palette;

    inline void set_pixmap(QLabel *icon)
    {
        QPixmap block(this->block_size);
        block.fill(this->color());
        icon->setPixmap(block);
    }
    inline QLayout *component_slider(QSlider *slider)
    {
        QHBoxLayout *layout = new QHBoxLayout();

        slider->setProperty("neutral", true);
        slider->setTracking(false);
        slider->setRange(0, 255);

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
