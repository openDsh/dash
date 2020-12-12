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
    static const QSize icon_20;
    static const QSize icon_22;
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
    inline void set_mode(bool mode) { this->mode = mode; }

    inline void set_scale(double scale) { this->scale = scale; }

    QIcon make_button_icon(QString name, QPushButton *button, QString alt_name = QString());
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
    QPalette palette;
    QMap<QString, QString> stylesheets;
    bool mode = false;
    double scale = 1.0;

    void set_palette();
    QString parse_stylesheet(QString file);
    QString scale_stylesheet(QString stylesheet);
    QIcon themed_button_icon(QIcon icon, QAbstractButton *button);

    inline QColor get_base_color() { return this->mode ? QColor(255, 255, 255) : QColor(0, 0, 0); }

   signals:
    void mode_updated(bool mode);
};
