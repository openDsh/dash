#ifndef COLOR_LABEL_HPP_
#define COLOR_LABEL_HPP_

#include <QWidget>
#include <QLabel>

#include <app/theme.hpp>

class ColorLabel : public QWidget {
    Q_OBJECT

   public:
    ColorLabel(QSize block_size, QWidget *parent = nullptr);
    void update(QString color);

    inline QString text() { return this->name->text(); }
    inline void setFont(QFont font) { this->name->setFont(font); }

   private:
    QLabel *icon;
    QLabel *name;
    Theme *theme;
};

#endif
