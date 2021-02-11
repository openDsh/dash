#pragma once

#include <QFrame>
#include <QString>
#include <QWidget>

class Arbiter;

class QuickView {
   public:
    QuickView(Arbiter &arbiter, QString name, QWidget *widget);

    virtual void init() = 0;

    const QString &name() { return this->name_; }
    QWidget *widget() { return this->widget_; }

   protected:
    Arbiter &arbiter;

   private:
    const QString name_;
    QWidget *widget_;
};

class NullQuickView : public QFrame, public QuickView {
    Q_OBJECT

   public:
    NullQuickView(Arbiter &arbiter);

    void init() override;
};

class VolumeQuickView : public QFrame, public QuickView {
    Q_OBJECT

   public:
    VolumeQuickView(Arbiter &arbiter);

    void init() override;
};

class BrightnessQuickView : public QFrame, public QuickView {
    Q_OBJECT

   public:
    BrightnessQuickView(Arbiter &arbiter);

    void init() override;
};
