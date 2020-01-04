#ifndef THEME_HPP_
#define THEME_HPP_

#include <QApplication>
#include <QDebug>
#include <QMainWindow>
#include <QMap>
#include <QObject>
#include <QPair>
#include <QPushButton>
#include <QString>

class Theme : public QObject {
    Q_OBJECT

   public:
    explicit Theme(QObject *parent = 0);

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

    void add_tab_icon(QString name, int index, int orientation = 0);
    void add_button_icon(QString name, QPushButton *button, QString active_name = QString());
    void update();

    static Theme *get_instance();

   private:
    const QString ICON_PATH = ":/icons/";

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
    QString color = "blue";

    QMap<QString, QList<QPair<int, QIcon>>> tab_icons;
    QMap<QString, QList<QPair<QPushButton *, QIcon>>> button_icons;

    QMap<QString, QString> stylesheets;

    bool mode = false;

    void set_palette();

    QString parse_stylesheet(QString);

    QPixmap create_pixmap_variant(QPixmap &base, qreal opacity);

   signals:
    void icons_updated(QList<QPair<int, QIcon>> &, QList<QPair<QPushButton *, QIcon>> &);
    void color_updated(QMap<QString, QColor> &);
};

#endif
