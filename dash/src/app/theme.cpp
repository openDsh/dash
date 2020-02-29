#include <QApplication>
#include <QFile>
#include <QIcon>
#include <QObject>
#include <QPainter>
#include <QPair>
#include <QPixmap>
#include <QTextStream>
#include <QTransform>

#include "app/theme.hpp"

const QFont Theme::font_14 = QFont("Montserrat", 14);
const QFont Theme::font_16 = QFont("Montserrat", 16);
const QFont Theme::font_18 = QFont("Montserrat", 18);
const QFont Theme::font_36 = QFont("Montserrat", 36);

const QSize Theme::icon_16 = QSize(16, 16);
const QSize Theme::icon_24 = QSize(24, 24);
const QSize Theme::icon_32 = QSize(32, 32);
const QSize Theme::icon_36 = QSize(36, 36);
const QSize Theme::icon_42 = QSize(42, 42);
const QSize Theme::icon_48 = QSize(48, 48);
const QSize Theme::icon_56 = QSize(56, 56);
const QSize Theme::icon_84 = QSize(84, 84);
const QSize Theme::icon_96 = QSize(96, 96);

const QColor Theme::danger_color = QColor(211, 47, 47);
const QColor Theme::success_color = QColor(56, 142, 60);

Theme::Theme(QObject *parent) : QObject(parent), palette()
{
    this->stylesheets["light"] = this->parse_stylesheet(":/light.qss");
    this->stylesheets["dark"] = this->parse_stylesheet(":/dark.qss");

    qApp->setStyleSheet(this->stylesheets[this->mode ? "dark" : "light"]);

    this->set_palette();
}

QString Theme::parse_stylesheet(QString file)
{
    QString stylesheet;

    QFile *f = new QFile(file);
    f->open(QFile::ReadOnly | QFile::Text);
    QTextStream s(f);
    stylesheet = s.readAll();
    f->close();

    delete f;

    return stylesheet;
}

void Theme::set_palette()
{
    QColor new_color(this->colors[this->mode ? "dark" : "light"][this->color]);
    this->palette.setColor(QPalette::Base, new_color);
    new_color.setAlphaF(.5);
    this->palette.setColor(QPalette::AlternateBase, new_color);
    qApp->setPalette(this->palette);
}

QPixmap Theme::create_pixmap_variant(QPixmap &base, qreal opacity)
{
    QPixmap image(base.size());
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setOpacity(opacity);
    painter.drawPixmap(0, 0, base);
    painter.end();

    return image;
}

void Theme::add_tab_icon(QString name, int index, int orientation)
{
    QTransform t;
    t.rotate(orientation);

    QPixmap dark_base = QIcon(this->ICON_PATH + "dark/" + name + "-24px.svg").pixmap(512, 512).transformed(t);
    QPixmap dark_active = this->create_pixmap_variant(dark_base, .87);
    QPixmap dark_normal = this->create_pixmap_variant(dark_base, .54);
    QPixmap dark_disabled = this->create_pixmap_variant(dark_base, .38);

    QIcon dark_icon = QIcon(dark_normal);
    dark_icon.addPixmap(dark_active, QIcon::Active, QIcon::On);
    dark_icon.addPixmap(dark_disabled, QIcon::Disabled);
    this->tab_icons["dark"].append({index, dark_icon});

    QPixmap light_base = QIcon(this->ICON_PATH + "light/" + name + "-24px.svg").pixmap(512, 512).transformed(t);
    QPixmap light_active = this->create_pixmap_variant(light_base, 1);
    QPixmap light_normal = this->create_pixmap_variant(light_base, .7);
    QPixmap light_disabled = this->create_pixmap_variant(light_base, .5);

    QIcon light_icon = QIcon(light_normal);
    light_icon.addPixmap(light_active, QIcon::Active, QIcon::On);
    light_icon.addPixmap(light_disabled, QIcon::Disabled);
    this->tab_icons["light"].append({index, light_icon});

    this->update();
}

void Theme::add_button_icon(QString name, QPushButton *button, QString normal_name)
{
    bool checkable = button->isCheckable();

    QPixmap dark_base = QIcon(this->ICON_PATH + "dark/" + name + "-24px.svg").pixmap(512, 512);
    QPixmap light_base = QIcon(this->ICON_PATH + "light/" + name + "-24px.svg").pixmap(512, 512);

    QPixmap dark_active = this->create_pixmap_variant(dark_base, .87);
    QPixmap dark_disabled = this->create_pixmap_variant(dark_base, .38);

    QPixmap light_active = this->create_pixmap_variant(light_base, 1);
    QPixmap light_disabled = this->create_pixmap_variant(light_base, .5);

    QPixmap dark_normal;
    QPixmap light_normal;
    if (normal_name.isNull()) {
        dark_normal = checkable ? this->create_pixmap_variant(dark_base, .54) : dark_active;
        light_normal = checkable ? this->create_pixmap_variant(light_base, .7) : light_active;
    }
    else {
        QPixmap dark_normal_base = QIcon(this->ICON_PATH + "dark/" + normal_name + "-24px.svg").pixmap(512, 512);
        QPixmap light_normal_base = QIcon(this->ICON_PATH + "light/" + normal_name + "-24px.svg").pixmap(512, 512);

        dark_normal = this->create_pixmap_variant(dark_normal_base, .87);
        light_normal = this->create_pixmap_variant(light_normal_base, 1);
    }

    QIcon dark_icon = QIcon(dark_normal);
    dark_icon.addPixmap(dark_active, QIcon::Active, QIcon::On);
    dark_icon.addPixmap(dark_disabled, QIcon::Disabled);
    this->button_icons["dark"].append({button, dark_icon});

    QIcon light_icon = QIcon(light_normal);
    light_icon.addPixmap(light_active, QIcon::Active, QIcon::On);
    light_icon.addPixmap(light_disabled, QIcon::Disabled);
    this->button_icons["light"].append({button, light_icon});

    this->update();
}

void Theme::update()
{
    this->set_palette();
    qApp->setStyleSheet(this->stylesheets[this->mode ? "dark" : "light"]);

    emit icons_updated(this->tab_icons[this->mode ? "dark" : "light"],
                       this->button_icons[this->mode ? "dark" : "light"]);

    emit color_updated(this->colors[this->mode ? "dark" : "light"]);
}

Theme *Theme::get_instance()
{
    static Theme theme;
    return &theme;
}
