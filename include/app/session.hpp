#pragma once

#include <array>

#include <QColor>
#include <QFileInfo>
#include <QMap>
#include <QPluginLoader>
#include <QSettings>
#include <QString>
#include <QVector>
#include <QWidget>

#include "app/bluetooth.hpp"
#include "app/shortcuts.hpp"
#include "app/pages/openauto.hpp"
#include "app/pages/page.hpp"

class Arbiter;

// dummy type for now
using Server = uint8_t;

class Session {
   public:
    struct Theme {
        enum Mode {
            Light = 0,
            Dark,
            NUM_MODES
        };

        Mode mode;

        Theme(QSettings &settings);

        QColor &color(Mode mode) { return this->colors_[mode]; }
        QColor color(Mode mode) const { return this->colors_[mode]; }
        QColor &color() { return this->colors_[this->mode]; }
        QColor color() const { return this->colors_[this->mode]; }

       private:
        std::array<QColor, NUM_MODES> colors_;
    };

    struct System {
        struct Brightness {
            QPluginLoader loader;
            uint8_t value;

            Brightness(QSettings &settings);

           private:
            QMap<QString, QFileInfo *> plugins_;
        };

        static const char *VOLUME_CMD;
        static const char *SHUTDOWN_CMD;
        static const char *REBOOT_CMD;

        Server server;
        Bluetooth bluetooth;
        Brightness brightness;
        uint8_t volume;

        System(QSettings &settings);

        // shortcut inserter

       private:
        QMap<QString, Shortcut *> shortcuts_;
    };

    struct Layout {
        struct ControlBar {
            bool enabled;
            QWidget *quick_view;

            ControlBar(QSettings &settings);

           private:
            QMap<QString, QWidget *> quick_views_;
        };

        float scale;
        ControlBar control_bar;
        OpenAutoPage *openauto_page;
        Page *active_page;

        Layout(QSettings &settings, Arbiter &arbiter);

        const QVector<Page *> &pages() const { return this->pages_; }
        Page *page(int id) const { return this->pages_.value(id, nullptr); }
        int page_id(Page *page) const { return this->pages_.indexOf(page); }

       private:
        QVector<Page *> pages_;
    };

    Session(Arbiter &arbiter);

    friend class Arbiter;

   private:
    QSettings settings_;
    Theme theme_;
    System system_;
    Layout layout_;
};
