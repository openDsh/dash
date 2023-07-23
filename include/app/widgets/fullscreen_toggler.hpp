#pragma once

#include <QTime>

#include <QDebug>
#include <QDialog>

class Arbiter;

class FullscreenToggler {
   public:
    FullscreenToggler(Arbiter &arbiter, QString name, QWidget *widget);

    virtual void enable() = 0;
    virtual void disable() = 0;

    const QString &name() { return this->name_; }
    QWidget *widget() { return this->widget_; }

   protected:
    Arbiter &arbiter;

   private:
    const QString name_;
    QWidget *widget_;
};

class NullFullscreenToggler : public QFrame, public FullscreenToggler {
    Q_OBJECT

   public:
    NullFullscreenToggler(Arbiter &arbiter);

    void enable() override {}
    void disable() override {}
};

class BarFullscreenToggler : public QFrame, public FullscreenToggler {
    Q_OBJECT

   public:
    BarFullscreenToggler(Arbiter &arbiter);

    void enable() override { this->setVisible(true); }
    void disable() override { this->setVisible(false); }
};

class ButtonFullscreenToggler : public QDialog, public FullscreenToggler {
    Q_OBJECT

   public:
    ButtonFullscreenToggler(Arbiter &arbiter);

    void enable() override { this->show(); }
    void disable() override { this->close(); }

   protected:
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

   private:
    QPoint p;
    QPoint last_pos;
    QTime touch_start;
    QLabel *label;
};
