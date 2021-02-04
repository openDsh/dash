#pragma once

#include <BluezQt/Device>
#include <QColor>
#include <QObject>

#include "app/session.hpp"
#include "app/pages/page.hpp"

class Arbiter : public QObject {
    Q_OBJECT

   public:
    Arbiter();

    // void set_mode(Session::Theme::Mode mode);
    void toggle_mode();
    void set_color(const QColor &color);
    void set_brightness_plugin(QString plugin);
    void set_brightness(uint8_t brightness);
    void set_volume(uint8_t volume);
    void set_scale(double scale);
    // void set_control_bar(bool enable);
    void toggle_control_bar();
    void set_curr_quick_view(QuickView *quick_view);
    void set_curr_page(Page *page);
    // void set_page(Page *page, bool enable);
    void toggle_page(Page *page);
    // void set_cursor(bool cursor);
    void toggle_cursor();

    QSettings &settings() { return this->session_.settings_; }
    Session::Theme &theme() { return this->session_.theme_; }
    Session::System &system() { return this->session_.system_; }
    Session::Layout &layout() { return this->session_.layout_; }
    Session::Forge &forge() { return this->session_.forge_; }
    Session::Core &core() { return this->session_.core_; }
    void update() { this->session_.update(); }

   private:
    Session session_;

   signals:
    // void mode_changed(Session::Theme::mode mode);
    void mode_toggled();
    void color_changed(QColor color);
    void brightness_plugin_changed(QString plugin);
    void brightness_changed(uint8_t brightness);
    void volume_changed(uint8_t volume);
    void scale_changed(float scale);
    // void control_bar_changed(bool enabled);
    void control_bar_toggled();
    void curr_quick_view_changed(QuickView *quick_view);
    void curr_page_changed(Page *page);
    // void page_changed(Page *page, bool enabled);
    void page_toggled(Page *page);
    // void cursor_changed(bool cursor);
    void cursor_toggled();
};
