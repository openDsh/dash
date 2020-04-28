#ifndef BRIGHTNESS_HPP_
#define BRIGHTNESS_HPP_

#include <QMainWindow>
#include <QObject>
#include <QScreen>
#include <QFile>

class BrightnessModule : public QObject {
    Q_OBJECT

   public:
    BrightnessModule(bool enable_androidauto_update);

    virtual void set_brightness(int brightness) = 0;
    inline bool update_androidauto() { return this->enable_androidauto_update; }

   private:
    bool enable_androidauto_update;
};

class MockedBrightnessModule : public BrightnessModule {
    Q_OBJECT

   public:
    MockedBrightnessModule(QMainWindow *window);
    void set_brightness(int brightness);

   private:
    QMainWindow *window;
};

class RpiBrightnessModule : public BrightnessModule {
    Q_OBJECT

   public:
    RpiBrightnessModule();
    ~RpiBrightnessModule();

    void set_brightness(int brightness);

   private:
    const QString PATH = "/sys/class/backlight/rpi_backlight/brightness";

    QFile brightness_attribute;
};

class XBrightnessModule : public BrightnessModule {
    Q_OBJECT

   public:
    XBrightnessModule();
    void set_brightness(int brightness);

   private:
    QScreen *screen;
};

#endif
