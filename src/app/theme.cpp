#include <math.h>

#include <QApplication>
#include <QFile>
#include <QBitmap>
#include <QFontDatabase>
#include <QIcon>
#include <QObject>
#include <QPainter>
#include <QPair>
#include <QPixmap>
#include <QRegularExpression>
#include <QTextStream>
#include <QTransform>

#include "app/theme.hpp"
#include "app/config.hpp"

QFont Theme::font_10 = QFont("Montserrat", 10);
QFont Theme::font_12 = QFont("Montserrat", 12);
QFont Theme::font_14 = QFont("Montserrat", 14);
QFont Theme::font_16 = QFont("Montserrat", 16);
QFont Theme::font_18 = QFont("Montserrat", 18);
QFont Theme::font_24 = QFont("Montserrat", 24);
QFont Theme::font_36 = QFont("Montserrat", 36);
QFont Theme::font_72 = QFont("Montserrat", 72);

QSize Theme::icon_16 = QSize(16, 16);
QSize Theme::icon_20 = QSize(20, 20);
QSize Theme::icon_22 = QSize(22, 22);
QSize Theme::icon_24 = QSize(24, 24);
QSize Theme::icon_26 = QSize(26, 26);
QSize Theme::icon_28 = QSize(28, 28);
QSize Theme::icon_32 = QSize(32, 32);
QSize Theme::icon_36 = QSize(36, 36);
QSize Theme::icon_42 = QSize(42, 42);
QSize Theme::icon_48 = QSize(48, 48);
QSize Theme::icon_56 = QSize(56, 56);
QSize Theme::icon_84 = QSize(84, 84);
QSize Theme::icon_96 = QSize(96, 96);

const QColor Theme::danger_color = QColor(211, 47, 47);
const QColor Theme::success_color = QColor(56, 142, 60);

Theme::Theme() : QObject(qApp), palette()
{
    QFontDatabase::addApplicationFont(":/fonts/Titillium_Web/TitilliumWeb-Regular.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Montserrat/Montserrat-LightItalic.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Montserrat/Montserrat-Regular.ttf");

    this->stylesheets["light"] = this->parse_stylesheet(":/stylesheets/light.qss");
    this->stylesheets["dark"] = this->parse_stylesheet(":/stylesheets/dark.qss");
}

void Theme::set_scale(double scale)
{
    this->scale = scale;

    Theme::font_10.setPointSize(std::ceil(10 * this->scale));
    Theme::font_12.setPointSize(std::ceil(12 * this->scale));
    Theme::font_14.setPointSize(std::ceil(14 * this->scale));
    Theme::font_16.setPointSize(std::ceil(16 * this->scale));
    Theme::font_18.setPointSize(std::ceil(18 * this->scale));
    Theme::font_24.setPointSize(std::ceil(24 * this->scale));
    Theme::font_36.setPointSize(std::ceil(36 * this->scale));
    Theme::font_72.setPointSize(std::ceil(72 * this->scale));

    Theme::icon_16 *= this->scale;
    Theme::icon_20 *= this->scale;
    Theme::icon_22 *= this->scale;
    Theme::icon_24 *= this->scale;
    Theme::icon_26 *= this->scale;
    Theme::icon_28 *= this->scale;
    Theme::icon_32 *= this->scale;
    Theme::icon_36 *= this->scale;
    Theme::icon_42 *= this->scale;
    Theme::icon_48 *= this->scale;
    Theme::icon_56 *= this->scale;
    Theme::icon_84 *= this->scale;
    Theme::icon_96 *= this->scale;

    qApp->setFont(Theme::font_14);
}

QString Theme::parse_stylesheet(QString file)
{
    QFile f(file);
    f.open(QFile::ReadOnly | QFile::Text);
    QTextStream s(&f);
    QString stylesheet(s.readAll());
    f.close();

    return stylesheet;
}

QString Theme::scale_stylesheet(QString stylesheet)
{
    QRegularExpression px_regex(" (-?\\d+)px");
    QRegularExpressionMatchIterator i = px_regex.globalMatch(stylesheet);
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        if (match.hasMatch()) {
            int scaled_px = std::ceil(match.captured(1).toInt() * this->scale);
            stylesheet.replace(match.captured(), QString("%1px").arg(scaled_px));
        }
    }

    return stylesheet;
}

void Theme::set_palette()
{
    QColor color(Config::get_instance()->get_color());
    this->palette.setColor(QPalette::Base, color);
    color.setAlphaF(.5);
    this->palette.setColor(QPalette::AlternateBase, color);
    qApp->setPalette(this->palette);
}

QIcon Theme::themed_button_icon(QIcon icon, QAbstractButton *button)
{
    QSize size(512, 512);
    QBitmap icon_mask(icon.pixmap(size).createMaskFromColor(Qt::transparent));
    QBitmap alt_icon_mask;
    {
        QIcon alt_icon(button->property("alt_icon").value<QIcon>());
        if (!alt_icon.isNull())
            alt_icon_mask = alt_icon.pixmap(size).createMaskFromColor(Qt::transparent);
    }

    QColor base_color(this->get_base_color());
    base_color.setAlpha(this->mode ? 222 : 255);
    QColor accent_color(Config::get_instance()->get_color());
    accent_color.setAlpha(this->mode ? 222 : 255);

    QPixmap normal_on(size);
    normal_on.fill(!button->property("page").isNull() ? accent_color : base_color);
    normal_on.setMask(!alt_icon_mask.isNull() ? alt_icon_mask : icon_mask);

    QPixmap normal_off(size);
    {
        QColor color(base_color);
        if (!button->property("page").isNull())
            color.setAlpha(this->mode ? 134 : 162);
        normal_off.fill(color);
        normal_off.setMask(icon_mask);
    }

    QPixmap disabled_on(size);
    QPixmap disabled_off(size);
    {
        QColor color(base_color);
        color.setAlpha(this->mode ? 128 : 97);
        disabled_on.fill(color);
        disabled_on.setMask(!alt_icon_mask.isNull() ? alt_icon_mask : icon_mask);
        disabled_off.fill(color);
        disabled_off.setMask(icon_mask);
    }

    QIcon themed_icon;
    themed_icon.addPixmap(disabled_on, QIcon::Disabled, QIcon::On);
    themed_icon.addPixmap(disabled_off, QIcon::Disabled, QIcon::Off);
    themed_icon.addPixmap(normal_on, QIcon::Normal, QIcon::On);
    themed_icon.addPixmap(normal_off, QIcon::Normal, QIcon::Off);

    return themed_icon;
}

QIcon Theme::make_icon(QString name)
{
    return QIcon(QString(":/icons/%1.svg").arg(name));
}

QIcon Theme::make_button_icon(QString name, QPushButton *button, QString alt_name)
{
    if (!alt_name.isNull())
        button->setProperty("alt_icon", QVariant::fromValue(QIcon(QString(":/icons/%1.svg").arg(alt_name))));

    button->setProperty("themed_icon", true);
    return QIcon(QString(":/icons/%1.svg").arg(name));
}

void Theme::update()
{
    this->set_palette();
    qApp->setStyleSheet(this->scale_stylesheet(this->stylesheets[this->mode ? "dark" : "light"]));

    for (QWidget *widget : qApp->allWidgets()) {
        QAbstractButton *button = qobject_cast<QAbstractButton*>(widget);
        if ((button != nullptr) && !button->property("themed_icon").isNull())
            button->setIcon(this->themed_button_icon(button->icon(), button));
    }

    emit mode_updated(this->mode);
}

Theme *Theme::get_instance()
{
    static Theme theme;
    return &theme;
}
