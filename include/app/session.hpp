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
#include <QVector>
#include <QPalette>
#include <QWidget>

#include "app/action.hpp"
#include "app/bluetooth.hpp"
#include "app/pages/openauto.hpp"
#include "app/pages/page.hpp"
#include "app/quick_views/quick_view.hpp"

class Arbiter;

// dummy type for now
using Server = uint8_t;

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

        Mode mode;

        Theme(QSettings &settings);

        QPalette palette() const;
        void colorize(QAbstractButton *button) const;

        QColor &color(Mode mode) { return this->colors_[mode]; }
        QColor color(Mode mode) const { return this->colors_[mode]; }
        QColor &color() { return this->colors_[this->mode]; }
        QColor color() const { return this->colors_[this->mode]; }

       private:
        std::array<QColor, NUM_MODES> colors_;

        QColor base_color() const { return (this->mode == Light) ? QColor(0, 0, 0) : QColor(255, 255, 255); }
    };

    struct System {
        static const char *VOLUME_CMD;
        static const char *SHUTDOWN_CMD;
        static const char *REBOOT_CMD;

        struct Brightness {
            QString plugin;
            uint8_t value;

            Brightness(QSettings &settings);

            void load_plugin();
            void set();
            const QList<QString> plugins() const { return this->plugins_.keys(); }

           private:
            QPluginLoader loader_;
            QMap<QString, QFileInfo> plugins_;
        };

        Server server;
        Bluetooth bluetooth;
        Brightness brightness;
        uint8_t volume;

        System(QSettings &settings);

        void set_volume() const;
    };

    struct Layout {
        struct ControlBar {
            bool enabled;
            QuickView *curr_quick_view;

            ControlBar(QSettings &settings, Arbiter &arbiter);

            const QVector<QuickView *> &quick_views() const { return this->quick_views_; }
            QuickView *quick_view(int id) const { return this->quick_views_.value(id, nullptr); }
            int quick_view_id(QuickView *quick_view) const { return this->quick_views_.indexOf(quick_view); }

           private:
            QVector<QuickView *> quick_views_;
        };

        double scale;
        ControlBar control_bar;
        OpenAutoPage *openauto_page;
        Page *curr_page;

        Layout(QSettings &settings, Arbiter &arbiter);

        const QVector<Page *> &pages() const { return this->pages_; }
        Page *page(int id) const { return this->pages_.value(id, nullptr); }
        int page_id(Page *page) const { return this->pages_.indexOf(page); }

       private:
        QVector<Page *> pages_;
    };

    struct Forge {
        static QFrame *br(bool vertical = false);
        static void to_touch_scroller(QAbstractScrollArea *area);
        static void symbolize(QAbstractButton *button);

        Forge(Arbiter &arbiter);

        void iconize(QString name, QAbstractButton *button, uint8_t size, bool dynamic = false) const;
        void iconize(QString name, QString alt_name, QAbstractButton *button, uint8_t size, bool dynamic = false) const;
        QFont font(int size, bool mono = false) const;
        QWidget *brightness_slider(bool buttons = true) const;
        QWidget *volume_slider(bool buttons = true) const;

       private:
        Arbiter &arbiter_;
    };

    struct Core {
        bool cursor;

        Core(QSettings &settings, Arbiter &arbiter);

        QString stylesheet(Theme::Mode mode, float scale) const;
        void set_cursor() const;

        const QVector<Action *> &actions() const { return this->actions_; }
        Action *action(int id) const { return this->actions_.value(id, nullptr); }
        int action_id(Action *action) const { return this->actions_.indexOf(action); }
        QString stylesheet(Theme::Mode mode) const { return this->stylesheets_[mode]; }

       private:
        std::array<QString, Theme::NUM_MODES> stylesheets_;
        QVector<Action *> actions_;

        QString parse_stylesheet(QString path) const;
    };

    Session(Arbiter &arbiter);

    void update();

    friend class Arbiter;

   private:
    QSettings settings_;
    Theme theme_;
    System system_;
    Layout layout_;
    Forge forge_;
    Core core_;
};
