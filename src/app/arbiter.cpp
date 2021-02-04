#include "app/session.hpp"

#include "app/arbiter.hpp"

Arbiter::Arbiter()
    : QObject()
    , session_(*this)
{
}

void Arbiter::toggle_mode()
{
    auto mode = (this->theme().mode == Session::Theme::Light) ? Session::Theme::Dark : Session::Theme::Light;
    this->theme().mode = mode;
    this->settings().setValue("Theme/mode", mode);

    this->session_.update();

    emit mode_toggled();
    emit color_changed(this->theme().color());
}

void Arbiter::set_color(const QColor &color)
{
    this->theme().color() = color;
    if (this->theme().mode == Session::Theme::Light)
        this->settings().setValue("Theme/Color/light", color.name());
    else
        this->settings().setValue("Theme/Color/dark", color.name());

    this->session_.update();

    emit color_changed(color);
}

void Arbiter::set_brightness_plugin(QString plugin)
{
    this->system().brightness.plugin = plugin;
    this->settings().setValue("System/Brightness/plugin", plugin);

    this->system().brightness.load_plugin();

    emit brightness_plugin_changed(plugin);
}

void Arbiter::set_brightness(uint8_t brightness)
{
    this->system().brightness.value = brightness;
    this->settings().setValue("System/Brightness/value", brightness);

    this->system().brightness.set();

    emit brightness_changed(brightness);
}

void Arbiter::set_volume(uint8_t volume)
{
    this->system().volume = volume;
    this->settings().setValue("System/volume", volume);

    this->system().set_volume();

    emit volume_changed(volume);
}

void Arbiter::set_scale(double scale)
{
    this->layout().scale = scale;
    this->settings().setValue("Layout/scale", scale);

    this->scale_changed(scale);
}

void Arbiter::toggle_control_bar()
{
    auto enabled = !this->layout().control_bar.enabled;
    this->layout().control_bar.enabled = enabled;
    this->settings().setValue("Layout/ControlBar/enabled", enabled);

    emit control_bar_toggled();
}

void Arbiter::set_curr_quick_view(QuickView *quick_view)
{
    this->layout().control_bar.curr_quick_view = quick_view;
    this->settings().setValue("Layout/ControlBar/quick_view", this->layout().control_bar.quick_view_id(quick_view));

    emit curr_quick_view_changed(quick_view);
}

void Arbiter::set_curr_page(Page *page)
{
    this->layout().curr_page = page;

    emit curr_page_changed(page);
}

void Arbiter::toggle_page(Page *page)
{
    page->toggle();
    this->settings().beginGroup("Layout");
    this->settings().beginGroup("Page");
    this->settings().setValue(QString::number(this->layout().page_id(page)), page->enabled());
    this->settings().endGroup();
    this->settings().endGroup();

    emit page_toggled(page);
}

void Arbiter::toggle_cursor()
{
    auto cursor = !this->session_.core_.cursor;
    this->session_.core_.cursor = cursor;
    this->settings().setValue("Core/cursor", cursor);

    this->core().set_cursor();

    emit cursor_toggled();
}
