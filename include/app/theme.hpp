#ifndef THEME_HPP_
#define THEME_HPP_

#include <QAbstractScrollArea>
#include <QFrame>
#include <QMap>
#include <QObject>
#include <QPair>
#include <QPushButton>
#include <QScroller>
#include <QScrollerProperties>
#include <QString>
#include <QVariant>

typedef QPair<int, QIcon> tab_icon_t;
typedef QPair<QPushButton *, QIcon> button_icon_t;

class Theme : public QObject {
    Q_OBJECT

   public:
    enum Orientation { BOTTOM, RIGHT };

    static const QFont font_14;
    static const QFont font_16;
    static const QFont font_18;
    static const QFont font_36;

    static const QSize icon_16;
    static const QSize icon_24;
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

    inline void set_mode(bool mode)
    {
        this->mode = mode;
        this->update();
    }
    inline void set_color(QString color)
    {
        this->color = color;
        this->update();
    }
    inline const QMap<QString, QColor> get_colors() { return this->colors[this->mode ? "dark" : "light"]; }
    inline const QColor get_color(QString color) { return this->colors[this->mode ? "dark" : "light"][color]; }

    void add_tab_icon(QString name, int index, Qt::Orientation orientation = Qt::Orientation::Horizontal);
    void add_button_icon(QString name, QPushButton *button, QString active_name = QString());
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

    void set_palette();
    QString parse_stylesheet(QString file);
    QPixmap create_pixmap_variant(QPixmap &base, qreal opacity);

   signals:
    void icons_updated(QList<tab_icon_t> &, QList<button_icon_t> &);
    void color_updated();
};

#endif
