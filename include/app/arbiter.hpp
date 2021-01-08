#pragma once

#include <QColor>
#include <QObject>

#include "app/session.hpp"
#include "app/pages/page.hpp"

class Arbiter : public QObject {
    Q_OBJECT

   public:
    Arbiter();

    Session &session() { return session_; }
    Session::Theme &theme() { return session_.theme_; }
    Session::System &system() { return session_.system_; }
    Session::Layout &layout() { return session_.layout_; }

    // void set_mode(Session::Theme::Mode mode);
    void toggle_mode();
    void set_color(const QColor &color);

    void set_brightness(uint8_t brightness);
    void set_volume(uint8_t volume);

    void set_active_page(Page *page);
    // void set_page(Page *page, bool enable);
    void toggle_page(Page *page);

   private:
    Session session_;

   signals:
    void mode_toggled();
    void color_changed(QColor color);
    void brightness_changed(uint8_t brightness);
    void volume_changed(uint8_t brightness);
    void active_page_changed(Page *page);
    void page_toggled(Page *page);
};
