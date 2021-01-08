#include "app/session.hpp"

#include "app/arbiter.hpp"

Arbiter::Arbiter()
    : QObject()
    , session_(*this)
{
}

void Arbiter::toggle_mode()
{
    auto mode = (this->session_.theme_.mode == Session::Theme::Light) ? Session::Theme::Dark : Session::Theme::Light;
    this->session_.theme_.mode = mode;
    this->session_.settings_.setValue("Theme/mode", mode);

    emit mode_toggled();
}

void Arbiter::set_color(const QColor &color)
{
    this->session_.theme_.color() = color;
    if (this->session_.theme_.mode == Session::Theme::Light)
        this->session_.settings_.setValue("Theme/Color/light", color.name());
    else
        this->session_.settings_.setValue("Theme/Color/dark", color.name());

    emit color_changed(color);
}

void Arbiter::set_brightness(uint8_t brightness)
{
    this->session_.system_.brightness.value = brightness;
    this->session_.settings_.setValue("System/Brightness/value", brightness);

    emit brightness_changed(brightness);
}

void Arbiter::set_volume(uint8_t volume)
{
    this->session_.system_.volume = volume;
    this->session_.settings_.setValue("System/volume", volume);

    emit volume_changed(volume);
}

void Arbiter::set_active_page(Page *page)
{
    this->session_.layout_.active_page = page;

    emit active_page_changed(page);
}

void Arbiter::toggle_page(Page *page)
{
    page->toggle();
    this->session_.settings_.beginGroup("Layout");
    this->session_.settings_.beginGroup("Page");
    this->session_.settings_.setValue(QString::number(this->session_.layout_.page_id(page)), page->enabled());
    this->session_.settings_.endGroup();
    this->session_.settings_.endGroup();

    emit page_toggled(page);
}
