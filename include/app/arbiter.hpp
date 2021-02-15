#pragma once

#include <BluezQt/Device>
#include <QColor>
#include <QMainWindow>
#include <QObject>
#include <QString>

#include "app/action.hpp"
#include "app/session.hpp"
#include "app/pages/page.hpp"


class Arbiter : public QObject {
    Q_OBJECT

   public:
    Arbiter(QMainWindow *window);
    void set_mode(Session::Theme::Mode mode);
    void toggle_mode();
    void set_color(const QColor &color);
    void set_scale(double scale);
    void set_control_bar(bool enabled);
    void set_curr_quick_view(QuickView *quick_view);
    void set_curr_page(Page *page);
    void set_page(Page *page, bool enabled);
    void set_brightness_plugin(QString plugin);
    void set_brightness(uint8_t brightness);
    void decrease_brightness(uint8_t val);
    void increase_brightness(uint8_t val);
    void set_volume(uint8_t volume);
    void decrease_volume(uint8_t val);
    void increase_volume(uint8_t val);
    void set_cursor(bool enabled);
    void set_action(Action *action, QString key);

    QMainWindow *window() { return this->window_; }
    QSettings &settings() { return this->session_.settings_; }
    Session::Theme &theme() { return this->session_.theme_; }
    Session::Layout &layout() { return this->session_.layout_; }
    Session::System &system() { return this->session_.system_; }
    Session::Forge &forge() { return this->session_.forge_; }
    Session::Core &core() { return this->session_.core_; }
    void update() { this->session_.update(); }

   private:
    QMainWindow *window_;
    Session session_;

   signals:
    void mode_changed(Session::Theme::Mode mode);
    void color_changed(QColor color);
    void scale_changed(float scale);
    void control_bar_changed(bool enabled);
    void curr_quick_view_changed(QuickView *quick_view);
    void curr_page_changed(Page *page);
    void page_changed(Page *page, bool enabled);
    void brightness_plugin_changed(QString plugin);
    void brightness_changed(uint8_t brightness);
    void volume_changed(uint8_t volume);
    void cursor_changed(bool enabled);
    void action_changed(Action *action, QString key);
};
