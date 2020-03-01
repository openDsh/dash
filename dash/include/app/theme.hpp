#ifndef THEME_HPP_
#define THEME_HPP_

#include <QFrame>
#include <QMap>
#include <QObject>
#include <QPair>
#include <QPushButton>
#include <QString>

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

    void add_tab_icon(QString name, int index, Qt::Orientation orientation = Qt::Orientation::Horizontal);
    void add_button_icon(QString name, QPushButton *button, QString active_name = QString());
    void update();

    inline static QFrame *br(QWidget *parent = nullptr, bool vert = false)
    {
        QFrame *br = new QFrame(parent);
        br->setLineWidth(1);
        br->setFrameShape((vert) ? QFrame::VLine : QFrame::HLine);
        br->setFrameShadow(QFrame::Plain);

        return br;
    }
    static Theme *get_instance();

   private:
    QMap<QString, QMap<QString, QColor>> colors = {{"light",
                                                    {{"blue", QColor(33, 150, 243)},
                                                     {"red", QColor(244, 67, 54)},
                                                     {"green", QColor(76, 175, 80)},
                                                     {"orange", QColor(255, 152, 0)},
                                                     {"steel", QColor(96, 125, 139)}}},
                                                   {"dark",
                                                    {{"blue", QColor(144, 202, 249)},
                                                     {"red", QColor(239, 154, 154)},
                                                     {"green", QColor(165, 214, 167)},
                                                     {"orange", QColor(255, 204, 128)},
                                                     {"steel", QColor(176, 190, 197)}}}};

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
    void icons_updated(QList<tab_icon_t> &tab_icons, QList<button_icon_t> &button_icons);
    void color_updated(QMap<QString, QColor> &colors);
};

#endif
