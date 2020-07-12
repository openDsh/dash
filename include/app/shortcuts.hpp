#pragma once

#include <QApplication>
#include <QFile>
#include <QFileSystemWatcher>
#include <QKeyEvent>
#include <QPushButton>
#include <QShortcut>
#include <QString>
#include <QWidget>

class GpioWatcher : public QObject {
    Q_OBJECT

   public:
    GpioWatcher(QObject *parent = nullptr);

    inline void enable() { this->watcher->blockSignals(false); }
    inline void disable() { this->watcher->blockSignals(true); }

   private:
    QFileSystemWatcher *watcher;

   signals:
    void gpio_triggered(QString gpio);
};

class ShortcutInput : public QPushButton {
    Q_OBJECT

   public:
    ShortcutInput(QString shortcut, QWidget *parent = nullptr);

   protected:
    void keyPressEvent(QKeyEvent *event);
    inline void focusInEvent(QFocusEvent *) { this->gpio_watcher->enable(); }
    inline void focusOutEvent(QFocusEvent *) { this->gpio_watcher->disable(); }

   private:
    GpioWatcher *gpio_watcher;

   signals:
    void shortcut_updated(QString shortcut);
};

class Shortcut : public QObject {
    Q_OBJECT

   public:
    Shortcut(QString shortcut, QWidget *parent);
    ~Shortcut();

    inline QString to_str() { return this->shortcut; }
    void set_shortcut(QString shortcut);

   private:
    QString shortcut;
    QShortcut *key;
    QFileSystemWatcher *gpio;
    QFile gpio_value_attribute;
    int gpio_active_low;

   signals:
    void activated();
};

class Shortcuts : public QObject {
    Q_OBJECT

   public:
    Shortcuts() : QObject(qApp) {}

    void add_shortcut(QString id, QString description, Shortcut *shortcut);

    inline void update_shortcut(QString id, QString shortcut) { this->shortcuts[id].second->set_shortcut(shortcut); }
    inline QMap<QString, QPair<QString, Shortcut *>> get_shortcuts() { return this->shortcuts; }

    static Shortcuts *get_instance();

   private:
    QMap<QString, QPair<QString, Shortcut *>> shortcuts;

   signals:
    void shortcut_added(QString id, QString description, Shortcut *shortcut);
};

