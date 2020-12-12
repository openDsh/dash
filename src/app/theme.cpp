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

const QFont Theme::font_10 = QFont("Montserrat", 10);
const QFont Theme::font_12 = QFont("Montserrat", 12);
const QFont Theme::font_14 = QFont("Montserrat", 14);
const QFont Theme::font_16 = QFont("Montserrat", 16);
const QFont Theme::font_18 = QFont("Montserrat", 18);
const QFont Theme::font_24 = QFont("Montserrat", 24);
const QFont Theme::font_36 = QFont("Montserrat", 36);

const QSize Theme::icon_16 = QSize(16, 16);
const QSize Theme::icon_20 = QSize(20, 20);
const QSize Theme::icon_22 = QSize(22, 22);
const QSize Theme::icon_24 = QSize(24, 24);
const QSize Theme::icon_26 = QSize(26, 26);
const QSize Theme::icon_28 = QSize(28, 28);
const QSize Theme::icon_32 = QSize(32, 32);
const QSize Theme::icon_36 = QSize(36, 36);
const QSize Theme::icon_42 = QSize(42, 42);
const QSize Theme::icon_48 = QSize(48, 48);
const QSize Theme::icon_56 = QSize(56, 56);
const QSize Theme::icon_84 = QSize(84, 84);
const QSize Theme::icon_96 = QSize(96, 96);

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
            color.setAlpha(this->mode ? 102 : 178);
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

QIcon Theme::make_button_icon(QString name, QPushButton *button, QString alt_name)
{
    button->setProperty("base_icon_size", QVariant::fromValue(button->iconSize()));
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
        QFont font = widget->font();
        font.setPointSize(std::ceil(font.pointSize() * this->scale));
        widget->setFont(font);

        QAbstractButton *button = qobject_cast<QAbstractButton*>(widget);
        if ((button != nullptr) && !button->icon().isNull()) {
            QVariant base_icon_size = button->property("base_icon_size");
            if (base_icon_size.isValid()) {
                button->setIconSize(base_icon_size.value<QSize>() * this->scale);
                if (!button->property("themed_icon").isNull())
                    button->setIcon(this->themed_button_icon(button->icon(), button));
            }
        }
    }

    emit mode_updated(this->mode);
}

Theme *Theme::get_instance()
{
    static Theme theme;
    return &theme;
}
