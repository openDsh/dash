#include "app/session.hpp"

#include "app/arbiter.hpp"

Arbiter::Arbiter(QMainWindow *window)
    : QObject()
    , window_(window)
    , session_(*this)
{
}

void Arbiter::set_mode(Session::Theme::Mode mode)
{
    this->theme().mode = mode;
    this->settings().setValue("Theme/mode", mode);

    this->session_.update();

    emit mode_changed(mode);
    emit color_changed(this->theme().color());
}

void Arbiter::toggle_mode()
{
    auto mode = (this->theme().mode == Session::Theme::Light) ? Session::Theme::Dark : Session::Theme::Light;
    this->set_mode(mode);
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

void Arbiter::set_scale(double scale)
{
    this->layout().scale = scale;
    this->settings().setValue("Layout/scale", scale);

    emit scale_changed(scale);
}

void Arbiter::set_status_bar(bool enabled)
{
    this->layout().status_bar = enabled;
    this->settings().setValue("Layout/status_bar", enabled);

    emit status_bar_changed(enabled);
}

void Arbiter::set_control_bar(bool enabled)
{
    this->layout().control_bar.enabled = enabled;
    this->settings().setValue("Layout/ControlBar/enabled", enabled);

    emit control_bar_changed(enabled);
}

void Arbiter::set_curr_quick_view(QuickView *quick_view)
{
    auto id = this->layout().control_bar.quick_view_id(quick_view);
    if (id < 0)
        return;

    this->layout().control_bar.curr_quick_view = quick_view;
    this->settings().setValue("Layout/ControlBar/quick_view", id);

    emit curr_quick_view_changed(quick_view);
}

void Arbiter::set_curr_quick_view(int id)
{
    this->set_curr_quick_view(this->layout().control_bar.quick_view(id));
}

void Arbiter::set_curr_page(Page *page)
{
    if (this->layout().page_id(page) < 0 || !page->enabled())
        return;
    
    this->layout().curr_page = page;

    emit curr_page_changed(page);
}

void Arbiter::set_curr_page(int id)
{
    this->set_curr_page(this->layout().page(id));
}

void Arbiter::set_page(Page *page, bool enabled)
{
    auto id = this->layout().page_id(page);
    if (id < 0 || !page->toggleale())
        return;

    page->enable(enabled);
    this->settings().beginGroup("Layout");
    this->settings().beginGroup("Page");
    this->settings().setValue(QString::number(id), page->enabled());
    this->settings().endGroup();
    this->settings().endGroup();

    emit page_changed(page, enabled);
}

void Arbiter::set_brightness_plugin(QString plugin)
{
    if (!(this->system().brightness.plugins().contains(plugin) || (plugin == Session::System::Brightness::AUTO_PLUGIN)))
        return;

    this->system().brightness.plugin = plugin;
    this->settings().setValue("System/Brightness/plugin", plugin);

    this->system().brightness.reset();
    this->system().brightness.load();
    this->system().brightness.set();

    emit brightness_plugin_changed(plugin);
}

void Arbiter::set_brightness(uint8_t brightness)
{
    this->system().brightness.value = brightness;
    this->settings().setValue("System/Brightness/value", brightness);

    this->system().brightness.set();

    emit brightness_changed(brightness);
}

void Arbiter::decrease_brightness(uint8_t val)
{
    this->set_brightness(std::min(std::max(0, this->system().brightness.value - val), 255));
}

void Arbiter::increase_brightness(uint8_t val)
{
    this->set_brightness(std::min(std::max(0, this->system().brightness.value + val), 255));
}

void Arbiter::set_volume(uint8_t volume)
{
    this->system().volume = volume;
    this->settings().setValue("System/volume", volume);

    this->system().set_volume();

    emit volume_changed(volume);
}

void Arbiter::decrease_volume(uint8_t val)
{
    this->set_volume(std::min(std::max(0, this->system().volume - val), 100));
}

void Arbiter::increase_volume(uint8_t val)
{
    this->set_volume(std::min(std::max(0, this->system().volume + val), 100));
}

void Arbiter::set_cursor(bool enabled)
{
    this->session_.core_.cursor = enabled;
    this->settings().setValue("Core/cursor", enabled);

    this->core().set_cursor();

    emit cursor_changed(enabled);
}

void Arbiter::set_action(Action *action, QString key)
{
    auto id = QString::number(this->core().action_id(action));
    if (id < 0)
        return;

    action->set(key);
    this->settings().beginGroup("Core");
    this->settings().beginGroup("Action");
    if (key.isNull())
        this->settings().remove(id);
    else
        this->settings().setValue(id, key);
    this->settings().endGroup();
    this->settings().endGroup();

    emit action_changed(action, key);
}


void Arbiter::send_openauto_full_screen(bool fullscreen)
{
    emit openauto_full_screen(fullscreen);
}

void Arbiter::send_vehicle_data(QString gauge_id, double value)
{
    emit vehicle_update_data(gauge_id, value);
}