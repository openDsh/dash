#pragma once

#include <array>

#include <QAbstractButton>
#include <QAbstractScrollArea>
#include <QColor>
#include <QDir>
#include <QFileInfo>
#include <QFont>
#include <QFrame>
#include <QIcon>
#include <QList>
#include <QMap>
#include <QPluginLoader>
#include <QSettings>
#include <QString>
#include <QPalette>
#include <QWidget>

#include "app/action.hpp"
#include "app/pages/openauto.hpp"
#include "app/pages/page.hpp"
#include "app/quick_views/quick_view.hpp"
#include "app/services/bluetooth.hpp"
#include "app/services/clock.hpp"
#include "app/services/server.hpp"
#include "AAHandler.hpp"

class Arbiter;

class Session {
   public:
    static QDir plugin_dir(QString plugin);
    static QString fmt_plugin(QString plugin);

    struct Theme {
        enum Mode {
            Light = 0,
            Dark,
            NUM_MODES
        };

        static Mode from_str(QString mode);
        static QString to_str(Mode mode);

        Mode mode;

        Theme(QSettings &settings);
        QPalette palette() const;

        QColor &color(Mode mode) { return this->colors_[mode]; }
        QColor color(Mode mode) const { return this->colors_[mode]; }
        QColor &color() { return this->colors_[this->mode]; }
        QColor color() const { return this->colors_[this->mode]; }
        QColor base_color() const { return (this->mode == Light) ? QColor(0, 0, 0) : QColor(255, 255, 255); }

       private:
        std::array<QColor, NUM_MODES> colors_;
    };

    struct Layout {
        struct ControlBar {
            bool enabled;
            QuickView *curr_quick_view;

            ControlBar(QSettings &settings, Arbiter &arbiter);

            const QList<QuickView *> &quick_views() const { return this->quick_views_; }
            QuickView *quick_view(int id) const { return this->quick_views_.value(id, nullptr); }
            int quick_view_id(QuickView *quick_view) const { return this->quick_views_.indexOf(quick_view); }

           private:
            QList<QuickView *> quick_views_;
        };

        struct Fullscreen {
            enum Toggler {
                None = 0,
                Button,
                Bar
            };

            bool enabled;
            Toggler toggler;

            Fullscreen(QSettings &settings);
        };

        double scale;
        bool status_bar;
        Fullscreen fullscreen;
        ControlBar control_bar;
        OpenAutoPage *openauto_page;
        Page *curr_page;

        Layout(QSettings &settings, Arbiter &arbiter);
        Page *next_enabled_page(Page *page) const;

        const QList<Page *> &pages() const { return this->pages_; }
        Page *page(int id) const { return this->pages_.value(id, nullptr); }
        int page_id(Page *page) const { return this->pages_.indexOf(page); }

       private:
        QList<Page *> pages_;
    };

    struct System {
        static const char *VOLUME_CMD;
        static const char *SHUTDOWN_CMD;
        static const char *REBOOT_CMD;

        struct Brightness {
            static const char *AUTO_PLUGIN;

            QString plugin;
            uint8_t value;

            Brightness(QSettings &settings);
            void load();
            void set();
            void reset();
            const QList<QString> &plugins() const;

           private:
            struct PluginInfo {
                QString name;
                QString path;
                bool supported;
                uint8_t priority;

                bool operator<(const PluginInfo &rhs) const
                {
                    return (this->supported && !rhs.supported) ||
                        ((this->supported && rhs.supported) && (this->priority > rhs.priority));
                }
            };

            QPluginLoader loader_;
            QList<PluginInfo> plugin_infos_;
        };

        Clock clock;
        Server server;
        Bluetooth bluetooth;
        Brightness brightness;
        uint8_t volume;

        System(QSettings &settings, Arbiter &arbiter);
        void set_volume() const;
    };

    struct Forge {
        static QFrame *br(bool vertical = false);
        static void to_touch_scroller(QAbstractScrollArea *area);
        static void symbolize(QAbstractButton *button);

        Forge(Arbiter &arbiter);
        void iconize(QString name, QAbstractButton *button, uint8_t size) const;
        void iconize(QString name, QString alt_name, QAbstractButton *button, uint8_t size) const;
        void iconize(QIcon &icon, QAbstractButton *button, uint8_t size) const;
        QFont font(int size, bool mono = false) const;
        QWidget *brightness_slider(bool buttons = true) const;
        QWidget *volume_slider(bool buttons = true) const;

       private:
        Arbiter &arbiter_;
    };

    struct AndroidAuto {
        AAHandler *handler;

        AndroidAuto(Arbiter &arbiter);
    };

    struct Core {
        bool cursor;

        Core(QSettings &settings, Arbiter &arbiter);
        QString stylesheet(Theme::Mode mode, float scale) const;
        void set_cursor() const;

        const QList<Action *> &actions() const { return this->actions_; }
        Action *action(int id) const { return this->actions_.value(id, nullptr); }
        int action_id(Action *action) const { return this->actions_.indexOf(action); }
        QString stylesheet(Theme::Mode mode) const { return this->stylesheets_[mode]; }

       private:
        std::array<QString, Theme::NUM_MODES> stylesheets_;
        QList<Action *> actions_;

        QString parse_stylesheet(QString path) const;
    };

    Session(Arbiter &arbiter);
    void update();

    friend class Arbiter;

   private:
    QSettings settings_;
    Theme theme_;
    Layout layout_;
    System system_;
    Forge forge_;
    AndroidAuto android_auto_;
    Core core_;
};
