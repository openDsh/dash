#pragma once

#include <QAbstractScrollArea>
#include <QApplication>
#include <QFrame>
#include <QMap>
#include <QObject>
#include <QPair>
#include <QPushButton>
#include <QScroller>
#include <QScrollerProperties>
#include <QString>
#include <QVariant>
#include <tuple>

typedef QPair<QWidget *, QIcon> tab_icon_t;
typedef std::tuple<QPushButton *, QIcon, QSize> button_icon_t;

class Theme : public QObject {
    Q_OBJECT

   public:
    enum Orientation { BOTTOM, RIGHT };

    static const QFont font_10;
    static const QFont font_12;
    static const QFont font_14;
    static const QFont font_16;
    static const QFont font_18;
    static const QFont font_24;
    static const QFont font_36;

    static const QSize icon_16;
    static const QSize icon_24;
    static const QSize icon_26;
    static const QSize icon_28;
    static const QSize icon_32;
    static const QSize icon_36;
    static const QSize icon_42;
    static const QSize icon_48;
    static const QSize icon_56;
    static const QSize icon_84;
    static const QSize icon_96;

    static const QColor danger_color;
    static const QColor success_color;

    Theme();

    inline bool get_mode() { return this->mode; }
    inline void set_mode(bool mode)
    {
        this->mode = mode;
        this->update();
    }
    inline const QMap<QString, QColor> get_colors() { return this->colors[this->mode ? "dark" : "light"]; }
    inline const QColor get_color(QString color) { return this->colors[this->mode ? "dark" : "light"][color]; }
    inline void set_color(QString color)
    {
        this->color = color;
        this->update();
    }

    inline void set_scale(double scale)
    {
        this->scale = scale;
        this->update();
    }

    void add_tab_icon(QString name, QWidget *widget, Qt::Orientation orientation = Qt::Orientation::Horizontal);
    inline QIcon get_tab_icon(int idx) { return this->tab_icons[this->mode ? "dark" : "light"][idx].second; }
    inline QList<tab_icon_t> get_tab_icons() { return this->tab_icons[this->mode ? "dark" : "light"]; }
    void add_button_icon(QString name, QPushButton *button, QString active_name = QString());
    QIcon add_button_icon2(QString name, QPushButton *button, QString active_name = QString());
    void update();

    inline static QFrame *br(QWidget *parent = nullptr, bool vertical = false)
    {
        QFrame *br = new QFrame(parent);
        br->setLineWidth(2);
        br->setFrameShape(vertical ? QFrame::VLine : QFrame::HLine);
        br->setFrameShadow(QFrame::Plain);

        return br;
    }
    inline static void to_touch_scroller(QAbstractScrollArea *area)
    {
        QVariant policy =
            QVariant::fromValue<QScrollerProperties::OvershootPolicy>(QScrollerProperties::OvershootAlwaysOff);
        QScrollerProperties properties = QScroller::scroller(area->viewport())->scrollerProperties();
        properties.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, policy);
        properties.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, policy);

        area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        QScroller::grabGesture(area->viewport(), QScroller::LeftMouseButtonGesture);
        QScroller::scroller(area->viewport())->setScrollerProperties(properties);
    }
    static Theme *get_instance();

   private:
    QMap<QString, QMap<QString, QColor>> colors = {{"light",
                                                    {{"azure", QColor(33, 150, 243)},
                                                     {"rose", QColor(244, 67, 54)},
                                                     {"jade", QColor(76, 175, 80)},
                                                     {"fire", QColor(255, 152, 0)},
                                                     {"steel", QColor(96, 125, 139)},
                                                     {"lilac", QColor(103, 58, 183)}}},
                                                   {"dark",
                                                    {{"azure", QColor(144, 202, 249)},
                                                     {"rose", QColor(239, 154, 154)},
                                                     {"jade", QColor(165, 214, 167)},
                                                     {"fire", QColor(255, 204, 128)},
                                                     {"steel", QColor(176, 190, 197)},
                                                     {"lilac", QColor(179, 157, 219)}}}};

    QPalette palette;
    QString color;
    QMap<QString, QList<tab_icon_t>> tab_icons;
    QMap<QString, QList<button_icon_t>> button_icons;
    QMap<QString, QString> stylesheets;
    bool mode = false;
    double scale = 1.0;

    void set_palette();
    QString parse_stylesheet(QString file);
    QString scale_stylesheet(QString stylesheet);
    QPixmap create_pixmap_variant(QPixmap &base, qreal opacity);
    QIcon recolor_icon(QIcon icon, bool checkable);

   signals:
    void mode_updated(bool mode);
    void icons_updated(QList<tab_icon_t> &tab_icons, QList<button_icon_t> &button_icons, double scale);
    void color_updated();
};
