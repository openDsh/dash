#include <QCoreApplication>
#include <QFile>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QProcess>
#include <QPushButton>
#include <QRegularExpression>
#include <QSize>
#include <QSlider>
#include <QTextStream>

#include "app/arbiter.hpp"
#include "app/pages/camera.hpp"
#include "app/pages/vehicle.hpp"
#include "app/pages/launcher.hpp"
#include "app/pages/media.hpp"
#include "app/pages/settings.hpp"
#include "app/quick_views/combo.hpp"
#include "plugins/brightness_plugin.hpp"

#include "app/session.hpp"

QDir Session::plugin_dir(QString plugin)
{
    QDir plugin_dir(QCoreApplication::applicationDirPath());
    plugin_dir.cdUp();
    plugin_dir.cd("lib");
    plugin_dir.cd("plugins");
    plugin_dir.cd(plugin);

    return plugin_dir;
}

QString Session::fmt_plugin(QString plugin)
{
    plugin.remove(0, 3);
    plugin.replace("_", " ");

    return plugin;
}

Session::Theme::Theme(QSettings &settings)
    : mode(static_cast<Theme::Mode>(settings.value("Theme/mode", Session::Theme::Light).toUInt()))
{
    this->colors_[Session::Theme::Light] = QColor(settings.value("Theme/Color/light", "#000000").toString());
    this->colors_[Session::Theme::Dark] = QColor(settings.value("Theme/Color/dark", "#ffffff").toString());
}

QPalette Session::Theme::palette() const
{
    QPalette palette;
    QColor color(this->color());
    palette.setColor(QPalette::Base, color);
    color.setAlphaF(.5);
    palette.setColor(QPalette::AlternateBase, color);

    return palette;
}

void Session::Theme::colorize(QAbstractButton *button) const
{
    QSize size(512, 512);
    auto icon_mask(button->icon().pixmap(size).createMaskFromColor(Qt::transparent));
    QBitmap alt_icon_mask;
    {
        auto alt_icon(button->property("alt_icon").value<QIcon>());
        if (!alt_icon.isNull())
            alt_icon_mask = alt_icon.pixmap(size).createMaskFromColor(Qt::transparent);
    }

    QColor base_color(this->base_color());
    base_color.setAlpha((this->mode == Session::Theme::Light) ? 255 : 222);
    QColor accent_color(this->color());
    accent_color.setAlpha((this->mode == Session::Theme::Light) ? 255 : 222);

    QPixmap normal_on(size);
    normal_on.fill(!button->property("color_hint").isNull() ? accent_color : base_color);
    normal_on.setMask(!alt_icon_mask.isNull() ? alt_icon_mask : icon_mask);

    QPixmap normal_off(size);
    {
        QColor color(base_color);
        if (!button->property("color_hint").isNull())
            color.setAlpha((this->mode == Session::Theme::Light) ? 162 : 134);
        normal_off.fill(color);
        normal_off.setMask(icon_mask);
    }

    QPixmap disabled_on(size);
    QPixmap disabled_off(size);
    {
        QColor color(base_color);
        color.setAlpha((this->mode == Session::Theme::Light) ? 97 : 128);
        disabled_on.fill(color);
        disabled_on.setMask(!alt_icon_mask.isNull() ? alt_icon_mask : icon_mask);
        disabled_off.fill(color);
        disabled_off.setMask(icon_mask);
    }

    QIcon colored_icon;
    colored_icon.addPixmap(disabled_on, QIcon::Disabled, QIcon::On);
    colored_icon.addPixmap(disabled_off, QIcon::Disabled, QIcon::Off);
    colored_icon.addPixmap(normal_on, QIcon::Normal, QIcon::On);
    colored_icon.addPixmap(normal_off, QIcon::Normal, QIcon::Off);
    button->setIcon(colored_icon);
}

const char *Session::System::VOLUME_CMD = "amixer set Master %1% --quiet";
const char *Session::System::SHUTDOWN_CMD = "sudo shutdown -h now";
const char *Session::System::REBOOT_CMD = "sudo shutdown -r now";

Session::System::Brightness::Brightness(QSettings &settings)
    : plugin(settings.value("System/Brightness/plugin", "mocked").toString())
    , value(settings.value("System/Brightness/value", 255).toUInt())
    , loader_()
{
    for (const auto plugin : Session::plugin_dir("brightness").entryInfoList(QDir::Files)) {
        if (QLibrary::isLibrary(plugin.absoluteFilePath()))
            this->plugins_[Session::fmt_plugin(plugin.baseName())] = plugin;
    }
    this->load_plugin();
    this->set();
}

void Session::System::Brightness::load_plugin()
{
    if (this->loader_.isLoaded())
        this->loader_.unload();
    this->loader_.setFileName(this->plugins_[this->plugin].absoluteFilePath());
}

void Session::System::Brightness::set()
{
    if (auto plugin = qobject_cast<BrightnessPlugin *>(this->loader_.instance()))
        plugin->set(this->value);
}

Session::System::System(QSettings &settings)
    : server()
    , bluetooth(settings)
    , brightness(settings)
    , volume(settings.value("System/volume", 50).toUInt())
{
    this->set_volume();
}

void Session::System::set_volume() const
{
    auto process = new QProcess();
    process->start(QString(VOLUME_CMD).arg(this->volume));
    process->waitForFinished();
}

Session::Layout::ControlBar::ControlBar(QSettings &settings, Arbiter &arbiter)
    : enabled(settings.value("Layout/ControlBar/enabled", true).toBool())
    , curr_quick_view(nullptr)
{
    this->quick_views_ = {
        new NullQuickView(arbiter),
        new VolumeQuickView(arbiter),
        new BrightnessQuickView(arbiter),
        new ComboQuickView(arbiter)
    };

    this->curr_quick_view = this->quick_views_.value(settings.value("Layout/ControlBar/quick_view", 0).toInt());
}

Session::Layout::Layout(QSettings &settings, Arbiter &arbiter)
    : scale(settings.value("Layout/scale", 1.0).toDouble())
    , control_bar(settings, arbiter)
    , openauto_page(new OpenAutoPage(arbiter))
    , curr_page(nullptr)
{
    this->pages_ = {
        this->openauto_page,
        new MediaPage(arbiter),
        new VehiclePage(arbiter),
        new CameraPage(arbiter),
        new LauncherPage(arbiter),
        new SettingsPage(arbiter)
    };

    settings.beginGroup("Layout");
    settings.beginGroup("Page");
    for (int i = 0; i < this->pages().size(); i++) {
        if (!settings.value(QString::number(i), true).toBool())
            this->page(i)->toggle();
    }
    settings.endGroup();
    settings.endGroup();

    this->curr_page = this->pages_[0];
}

QFrame *Session::Forge::br(bool vertical)
{
    auto br = new QFrame();
    br->setLineWidth(2);
    br->setFrameShape(vertical ? QFrame::VLine : QFrame::HLine);
    br->setFrameShadow(QFrame::Plain);

    return br;
}

void Session::Forge::to_touch_scroller(QAbstractScrollArea *area)
{
    auto policy = QVariant::fromValue<QScrollerProperties::OvershootPolicy>(QScrollerProperties::OvershootAlwaysOff);
    auto properties = QScroller::scroller(area->viewport())->scrollerProperties();
    properties.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, policy);
    properties.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, policy);

    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QScroller::grabGesture(area->viewport(), QScroller::LeftMouseButtonGesture);
    QScroller::scroller(area->viewport())->setScrollerProperties(properties);
}

Session::Forge::Forge(Arbiter &arbiter)
    : arbiter_(arbiter)
{
}

void Session::Forge::iconize(QString name, QAbstractButton *button, uint8_t size, bool dynamic) const
{
    auto scaled = size * this->arbiter_.layout().scale;
    button->setIconSize(QSize(scaled, scaled));

    button->setProperty("iconized", true);
    button->setIcon(QIcon(QString(":/icons/%1.svg").arg(name)));
    if (dynamic)
        this->arbiter_.theme().colorize(button);
}

void Session::Forge::iconize(QString name, QString alt_name, QAbstractButton *button, uint8_t size, bool dynamic) const
{
    button->setProperty("alt_icon", QVariant::fromValue(QIcon(QString(":/icons/%1.svg").arg(alt_name))));
    this->iconize(name, button, size, dynamic);
}

QFont Session::Forge::font(int size, bool mono) const
{
    auto name = mono ? "Titillium Web" : "Montserrat";
    auto scaled = size * this->arbiter_.layout().scale;

    return QFont(name, scaled);
}

QWidget *Session::Forge::brightness_slider(bool buttons) const
{
    auto widget = new QWidget();
    auto layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto slider = new QSlider(Qt::Orientation::Horizontal);
    slider->setTracking(false);
    slider->setRange(76, 255);
    slider->setValue(this->arbiter_.system().brightness.value);
    QObject::connect(slider, &QSlider::valueChanged, [this](int position){ this->arbiter_.set_brightness(position); });
    QObject::connect(&this->arbiter_, &Arbiter::brightness_changed, [slider](int brightness){ slider->setValue(brightness); });

    if (buttons) {
        auto dim_button = new QPushButton();
        dim_button->setFlat(true);
        this->iconize("brightness_low", dim_button, 26);
        QObject::connect(dim_button, &QPushButton::clicked, [slider]{ slider->setValue(slider->value() - 18); });

        auto brighten_button = new QPushButton();
        brighten_button->setFlat(true);
        this->iconize("brightness_high", brighten_button, 26);
        QObject::connect(brighten_button, &QPushButton::clicked, [slider]{ slider->setValue(slider->value() + 18); });

        layout->addWidget(dim_button);
        layout->addWidget(brighten_button);
    }

    layout->insertWidget(1, slider, 4);

    return widget;
}

QWidget *Session::Forge::volume_slider(bool buttons) const
{
    auto widget = new QWidget();
    auto layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto slider = new QSlider(Qt::Orientation::Horizontal);
    slider->setTracking(false);
    slider->setRange(0, 100);
    slider->setValue(this->arbiter_.system().volume);
    QObject::connect(slider, &QSlider::valueChanged, [this](int position) { this->arbiter_.set_volume(position); });
    QObject::connect(&this->arbiter_, &Arbiter::volume_changed, [slider](int volume) { slider->setValue(volume); });

    if (buttons) {
        auto lower_button = new QPushButton();
        lower_button->setFlat(true);
        this->iconize("volume_down", lower_button, 26);
        QObject::connect(lower_button, &QPushButton::clicked, [slider]{ slider->setValue(slider->value() - 10); });

        auto raise_button = new QPushButton();
        raise_button->setFlat(true);
        this->iconize("volume_up", raise_button, 26);
        QObject::connect(raise_button, &QPushButton::clicked, [slider]{ slider->setValue(slider->value() + 10); });

        layout->addWidget(lower_button);
        layout->addWidget(raise_button);
    }

    layout->insertWidget(1, slider, 4);

    return widget;
}

Session::Core::Core(QSettings &settings, Arbiter &arbiter)
    : cursor(settings.value("Core/cursor", true).toBool())
{
    QFontDatabase::addApplicationFont(":/fonts/Titillium_Web/TitilliumWeb-Regular.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Montserrat/Montserrat-LightItalic.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Montserrat/Montserrat-Regular.ttf");

    if (qApp)
        qApp->setFont(arbiter.forge().font(14));
    this->set_cursor();

    this->stylesheets_[Session::Theme::Light] = this->parse_stylesheet(":/stylesheets/light.qss");
    this->stylesheets_[Session::Theme::Dark] = this->parse_stylesheet(":/stylesheets/dark.qss");
}

QString Session::Core::stylesheet(Theme::Mode mode, float scale) const
{
    QRegularExpression regex(" (-?\\d+)px");

    auto stylesheet = this->stylesheet(mode);
    auto it = regex.globalMatch(stylesheet);
    while (it.hasNext()) {
        auto match = it.next();
        if (match.hasMatch()) {
            int scaled_px = std::ceil(match.captured(1).toInt() * scale);
            stylesheet.replace(match.captured(), QString("%1px").arg(scaled_px));
        }
    }

    return stylesheet;
}

void Session::Core::set_cursor() const
{
    if (qApp)
        qApp->setOverrideCursor(this->cursor ? Qt::ArrowCursor : Qt::BlankCursor);
}

QString Session::Core::parse_stylesheet(QString path) const
{
    QFile file(path);
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);

    return stream.readAll();
}

Session::Session(Arbiter &arbiter)
    : settings_(QSettings::IniFormat, QSettings::UserScope, "dash")
    , theme_(settings_)
    , system_(settings_)
    , layout_(settings_, arbiter)
    , forge_(arbiter)
    , core_(settings_, arbiter)
{
    for (auto page : this->layout_.pages())
        page->init();
    for (auto quick_view : this->layout_.control_bar.quick_views())
        quick_view->init();
}

void Session::update()
{
    if (qApp) {
        qApp->setPalette(this->theme_.palette());
        qApp->setStyleSheet(this->core_.stylesheet(this->theme_.mode, this->layout_.scale));

        for (QWidget *widget : qApp->allWidgets()) {
            auto button = qobject_cast<QAbstractButton*>(widget);
            if ((button != nullptr) && !button->property("iconized").isNull())
                this->theme_.colorize(button);
        }
    }
}
