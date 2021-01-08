
#include "app/pages/camera.hpp"
#include "app/pages/vehicle.hpp"
#include "app/pages/launcher.hpp"
#include "app/pages/media.hpp"
#include "app/pages/settings.hpp"

#include "app/session.hpp"

Session::Theme::Theme(QSettings &settings)
    : mode(static_cast<Theme::Mode>(settings.value("Theme/mode", Theme::Light).toUInt()))
{
    this->colors_[Theme::Light] = QColor(settings.value("Theme/Color/light", "#000000").toString());
    this->colors_[Theme::Dark] = QColor(settings.value("Theme/Color/dark", "#ffffff").toString());
}

Session::System::Brightness::Brightness(QSettings &settings)
    : loader()
    , value(settings.value("System/Brightness/value", 255).toUInt())
{
    // fill brightness plugins
}

const char *Session::System::VOLUME_CMD = "amixer set Master %1% --quiet";
const char *Session::System::SHUTDOWN_CMD = "sudo shutdown -h now";
const char *Session::System::REBOOT_CMD = "sudo shutdown -r now";

Session::System::System(QSettings &settings)
    : server()
    , bluetooth()
    , brightness(settings)
    , volume(settings.value("System/volume", 50).toUInt())
{
}

Session::Layout::ControlBar::ControlBar(QSettings &settings)
    : enabled(settings.value("Layout/ControlBar/enabled", true).toBool())
    , quick_view(nullptr)
{
    // fill quick views
}

Session::Layout::Layout(QSettings &settings, Arbiter &arbiter)
    : scale(settings.value("Layout/scale", 1.0).toFloat())
    , control_bar(settings)
    , openauto_page(new OpenAutoPage(arbiter))
    , active_page(nullptr)
{
    this->pages_ = {
        this->openauto_page,
        new MediaPage(arbiter),
        new VehiclePage(arbiter),
        new CameraPage(arbiter),
        new LauncherPage(arbiter)
    };

    settings.beginGroup("Layout");
    settings.beginGroup("Page");
    for (int i = 0; i < this->pages().size(); i++) {
        if (!settings.value(QString::number(i), true).toBool())
            this->page(i)->toggle();
    }
    settings.endGroup();
    settings.endGroup();

    this->pages_.push_back(new SettingsPage(arbiter)); // maybe settings page has an init function we call after dash is ready to run
}

Session::Session(Arbiter &arbiter)
    : settings_(QSettings::IniFormat, QSettings::UserScope, "dash")
    , theme_(settings_)
    , system_(settings_)
    , layout_(settings_, arbiter)
{
}
