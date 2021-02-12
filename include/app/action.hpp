#pragma once

#include <functional>

#include <QApplication>
#include <QElapsedTimer>
#include <QFile>
#include <QFileSystemWatcher>
#include <QKeyEvent>
#include <QPushButton>
#include <QShortcut>
#include <QString>
#include <QRegExp>
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

    GPIONotifier(QObject *parent = nullptr);

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
    Action(Arbiter &arbiter, QString name, std::function<void()> callback, QWidget *parent);
    ~Action();

    void set(QString action);
    QWidget *input_widget();

    QString key() { return this->key_; }
    QString name() { return this->name_; }

   private:
    struct GPIO {
        QFileSystemWatcher watcher;
        QFile value;
        uint8_t active_low;

        GPIO(QObject *parent);
    };

    Arbiter &arbiter;
    QString name_;
    std::function<void()> callback;
    QString key_;
    QShortcut shortcut;
    GPIO gpio;
};
