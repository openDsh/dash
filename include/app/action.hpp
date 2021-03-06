#pragma once

#include <QCloseEvent>
#include <QFile>
#include <QFileSystemWatcher>
#include <QKeyEvent>
#include <QLabel>
#include <QObject>
#include <QRegExp>
#include <QShortcut>
#include <QString>
#include <QShowEvent>
#include <QWidget>

#include "app/widgets/dialog.hpp"

class Arbiter;

class GPIONotifier : public QObject {
    Q_OBJECT

   public:
    static const QRegExp GPIOX_REGEX;
    static const QString GPIO_DIR;
    static const QString GPIOX_DIR;
    static const QString GPIOX_VALUE_PATH;
    static const QString GPIOX_ACTIVE_LOW_PATH;

    GPIONotifier();

    void enable() { this->watcher.blockSignals(false); }
    void disable() { this->watcher.blockSignals(true); }

   private:
    QFileSystemWatcher watcher;

   signals:
    void triggered(QString gpio);
};

class ActionDialog : public Dialog {
    Q_OBJECT

   public:
    ActionDialog(Arbiter &arbiter);

    QString key() { return label->text(); }

   protected:
    void keyPressEvent(QKeyEvent *event);
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);

   private:
    GPIONotifier notifier;
    QLabel *label;
};

class Action : public QObject {
    Q_OBJECT

   public:
    Action(QString name, std::function<void()> func, QWidget *parent);
    void set(QString key);

    QString key() const { return this->key_; }
    QString name() const { return this->name_; }

   private:
    struct GPIO {
        QFileSystemWatcher watcher;
        QFile value;
        uint8_t active_low;

        GPIO();
        ~GPIO();
    };

    QShortcut shortcut;
    GPIO gpio;

    QString name_;
    QString key_;
};
