#include <algorithm>

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
#include "app/utilities/icon_engine.hpp"
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

Session::Theme::Mode Session::Theme::from_str(QString mode)
{
    // defaults to light mode if unknown
    return (mode == "Dark") ? Session::Theme::Dark : Session::Theme::Light;
}

QString Session::Theme::to_str(Session::Theme::Mode mode)
{
    switch (mode) {
        case Session::Theme::Light:
            return "Light";
            break;
        case Session::Theme::Dark:
            return "Dark";
            break;
        default:
            return QString();
    }
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
    auto color = this->color();
    palette.setColor(QPalette::Base, color);
    color.setAlphaF(.5);
    palette.setColor(QPalette::AlternateBase, color);

    return palette;
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
    for (int i = 0; i < this->pages_.size(); i++) {
        auto page = this->page(i);
        if (page->toggleale()) {
            if (!settings.value(QString::number(i), true).toBool())
                page->enable(false);
        }
    }
    settings.endGroup();
    settings.endGroup();

    for (auto page : this->pages_) {
        if (page->enabled()) {
            this->curr_page = page;
            break;
        }
    }
}

Page *Session::Layout::next_enabled_page(Page *page) const
{
    auto id = this->page_id(page);
    do {
        id = (id + 1) % this->pages_.size();
    } while (!this->page(id)->enabled());

    return this->page(id);
}

const char *Session::System::VOLUME_CMD = "amixer set Master %1% --quiet";
const char *Session::System::SHUTDOWN_CMD = "sudo shutdown -h now";
const char *Session::System::REBOOT_CMD = "sudo shutdown -r now";

const char *Session::System::Brightness::AUTO_PLUGIN = "auto";

Session::System::Brightness::Brightness(QSettings &settings)
    : plugin(settings.value("System/Brightness/plugin", Session::System::Brightness::AUTO_PLUGIN).toString())
    , value(settings.value("System/Brightness/value", 255).toUInt())
    , loader_()
{
    for (const auto file : Session::plugin_dir("brightness").entryInfoList(QDir::Files)) {
        auto path = file.absoluteFilePath();
        if (QLibrary::isLibrary(path)) {
            auto name = Session::fmt_plugin(file.baseName());
            if (auto plugin = qobject_cast<BrightnessPlugin *>(QPluginLoader(path).instance()))
                this->plugin_infos_.append({name, path, plugin->supported(), plugin->priority()});
        }
    }
    std::sort(this->plugin_infos_.begin(), this->plugin_infos_.end());

    this->load();
    this->set();
}

void Session::System::Brightness::load()
{
    if (this->loader_.isLoaded())
        this->loader_.unload();

    if ((this->plugin == Session::System::Brightness::AUTO_PLUGIN) && !this->plugin_infos_.isEmpty()) {
        this->loader_.setFileName(this->plugin_infos_.first().path);
    }
    else {
        auto it = std::find_if(this->plugin_infos_.begin(), this->plugin_infos_.end(), [this](PluginInfo &info){
            return info.name == this->plugin;
        });
        if (it != this->plugin_infos_.end())
            this->loader_.setFileName(it->path);
    }
}

void Session::System::Brightness::set()
{
    if (auto plugin = qobject_cast<BrightnessPlugin *>(this->loader_.instance()))
        plugin->set(this->value);
}

void Session::System::Brightness::reset()
{
    if (auto plugin = qobject_cast<BrightnessPlugin *>(this->loader_.instance()))
        plugin->set(255);
}

const QList<QString> &Session::System::Brightness::plugins() const
{
    // generates only once
    static const QList<QString> plugins = [this]{
        QList<QString> names;
        for (const auto info : this->plugin_infos_) {
            if (info.supported)
                names.append(info.name);
        }
        return names;
    }();
    return plugins;
}

Session::System::System(QSettings &settings, Arbiter &arbiter)
    : server(arbiter)
    , bluetooth(arbiter)
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

void Session::Forge::symbolize(QAbstractButton *button)
{
    auto policy = button->sizePolicy();
    policy.setRetainSizeWhenHidden(true);
    button->setSizePolicy(policy);
    button->setFocusPolicy(Qt::NoFocus);
    button->setAttribute(Qt::WA_TransparentForMouseEvents, true);
}

Session::Forge::Forge(Arbiter &arbiter)
    : arbiter_(arbiter)
{
}

void Session::Forge::iconize(QString name, QAbstractButton *button, uint8_t size) const
{
    this->iconize(name, QString(), button, size);
}

void Session::Forge::iconize(QString name, QString alt_name, QAbstractButton *button, uint8_t size) const
{
    QIcon icon(new IconEngine(this->arbiter_, QString(":/icons/%1.svg").arg(name), false));
    if (!alt_name.isNull())
        icon.addFile(QString(":/icons/%1.svg").arg(alt_name), QSize(), QIcon::Normal, QIcon::On);
    this->iconize(icon, button, size);
}

void Session::Forge::iconize(QIcon &icon, QAbstractButton *button, uint8_t size) const
{
    auto scaled = size * this->arbiter_.layout().scale;
    button->setIconSize(QSize(scaled, scaled));
    button->setIcon(icon);
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
    QObject::connect(slider, &QSlider::sliderReleased, [this, slider]{
        this->arbiter_.set_brightness(slider->sliderPosition());
    });
    QObject::connect(&this->arbiter_, &Arbiter::brightness_changed, [slider](int brightness){ slider->setValue(brightness); });

    if (buttons) {
        auto dim_button = new QPushButton();
        dim_button->setFlat(true);
        this->iconize("brightness_low", dim_button, 26);
        QObject::connect(dim_button, &QPushButton::clicked, [this]{ this->arbiter_.decrease_brightness(18); });

        auto brighten_button = new QPushButton();
        brighten_button->setFlat(true);
        this->iconize("brightness_high", brighten_button, 26);
        QObject::connect(brighten_button, &QPushButton::clicked, [this]{ this->arbiter_.increase_brightness(18); });

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
    QObject::connect(slider, &QSlider::sliderReleased, [this, slider]{ this->arbiter_.set_volume(slider->sliderPosition()); });
    QObject::connect(&this->arbiter_, &Arbiter::volume_changed, [slider](int volume) { slider->setValue(volume); });

    if (buttons) {
        auto lower_button = new QPushButton();
        lower_button->setFlat(true);
        this->iconize("volume_down", lower_button, 26);
        QObject::connect(lower_button, &QPushButton::clicked, [this]{ this->arbiter_.decrease_volume(10); });

        auto raise_button = new QPushButton();
        raise_button->setFlat(true);
        this->iconize("volume_up", raise_button, 26);
        QObject::connect(raise_button, &QPushButton::clicked, [this]{ this->arbiter_.increase_volume(10); });

        layout->addWidget(lower_button);
        layout->addWidget(raise_button);
    }

    layout->insertWidget(1, slider, 4);

    return widget;
}

Session::Core::Core(QSettings &settings, Arbiter &arbiter)
    : cursor(settings.value("Core/cursor", true).toBool())
{
    this->stylesheets_[Session::Theme::Light] = this->parse_stylesheet(":/stylesheets/light.qss");
    this->stylesheets_[Session::Theme::Dark] = this->parse_stylesheet(":/stylesheets/dark.qss");

    this->actions_ = {
        new Action("Toggle Dark Mode", [&arbiter]{ arbiter.toggle_mode(); }, arbiter.window()),
        new Action("Decrease Brightness", [&arbiter]{ arbiter.decrease_brightness(4); }, arbiter.window()),
        new Action("Increase Brightness", [&arbiter]{ arbiter.increase_brightness(4); }, arbiter.window()),
        new Action("Decrease Volume", [&arbiter]{ arbiter.decrease_volume(2); }, arbiter.window()),
        new Action("Increase Volume", [&arbiter]{ arbiter.increase_volume(2); }, arbiter.window())
    };

    for (auto page : arbiter.layout().pages()) {
        auto callback = [&arbiter, page]{ arbiter.set_curr_page(page); };
        this->actions_.append(new Action(QString("Show %1 Page").arg(page->name()), callback, arbiter.window()));
    }

    {
        auto callback = [&arbiter]{ arbiter.set_curr_page(arbiter.layout().next_enabled_page(arbiter.layout().curr_page)); };
        this->actions_.append(new Action("Cycle Page", callback, arbiter.window()));
    }

    settings.beginGroup("Core");
    settings.beginGroup("Action");
    for (auto i = 0; i < this->actions_.size(); i++) {
        auto key = settings.value(QString::number(i), QString()).toString();
        if (!key.isNull())
            this->action(i)->set(key);
    }
    settings.endGroup();
    settings.endGroup();

    QFontDatabase::addApplicationFont(":/fonts/Titillium_Web/TitilliumWeb-Regular.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Montserrat/Montserrat-LightItalic.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Montserrat/Montserrat-Regular.ttf");

    if (qApp)
        qApp->setFont(arbiter.forge().font(14));

    this->set_cursor();
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
    : settings_()
    , theme_(settings_)
    , layout_(settings_, arbiter)
    , system_(settings_, arbiter)
    , forge_(arbiter)
    , core_(settings_, arbiter)
{
}

void Session::update()
{
    if (qApp) {
        qApp->setPalette(this->theme_.palette());
        qApp->setStyleSheet(this->core_.stylesheet(this->theme_.mode, this->layout_.scale));
    }
}
