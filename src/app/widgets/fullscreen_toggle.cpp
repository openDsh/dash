#include <QLabel>

#include "app/widgets/fullscreen_toggle.hpp"
#include "app/arbiter.hpp"

FullscreenToggle::FullscreenToggle(Arbiter &arbiter)
    : QDialog(arbiter.window(), Qt::FramelessWindowHint)
    , arbiter(arbiter)
    , p()
    , last_pos()
    , touch_start()
    , label(new QLabel(" X "))
{
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    this->label->setMargin(2);
    this->label->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    this->label->setAlignment(Qt::AlignCenter);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(label);

    connect(&this->arbiter, &Arbiter::fullscreen_changed, [this](bool fullscreen){
        if (fullscreen && (this->arbiter.layout().fullscreen.toggler == Session::Layout::Fullscreen::Button))
            this->show();
        else
            this->close();
    });
    connect(&this->arbiter, &Arbiter::fullscreen_toggler_changed, [this](Session::Layout::Fullscreen::Toggler toggler){
        // this->setVisible(this->arbiter.layout().fullscreen.enabled && (toggler == Session::Layout::Fullscreen::Button));

        if (this->arbiter.layout().fullscreen.enabled && (toggler == Session::Layout::Fullscreen::Button))
            this->show();
        else
            this->close();
    });
    connect(&this->arbiter, &Arbiter::brightness_changed, [this](uint8_t brightness){
        this->setWindowOpacity(brightness / 510.0);
    });
}

QWidget *FullscreenToggle::bar(Arbiter &arbiter)
{
    auto widget = new QWidget();
    auto layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    auto bar = new QPushButton();
    connect(bar, &QPushButton::clicked, [&arbiter]{ arbiter.toggle_fullscreen(false); });
    bar->setFixedHeight(10 * arbiter.layout().scale);
    bar->setObjectName("FullscreenBar");

    layout->addStretch(4);
    layout->addWidget(bar, 2);
    layout->addStretch(4);

    return widget;
}

void FullscreenToggle::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    this->setWindowOpacity(this->arbiter.system().brightness.value / 510.0);
    this->move(this->last_pos);
}

void FullscreenToggle::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        this->arbiter.toggle_fullscreen(false);
    else
        QDialog::keyPressEvent(event);
}

void FullscreenToggle::closeEvent(QCloseEvent *event)
{
    this->last_pos = this->pos();
    this->arbiter.window()->activateWindow();
}

void FullscreenToggle::mousePressEvent(QMouseEvent *event)
{
    this->setWindowOpacity(this->arbiter.system().brightness.value / 255.0);
    QFont f = this->label->font();
    f.setPointSize(f.pointSize() * 1.5);
    this->label->setFont(f);
    this->p = event->pos();
    this->touch_start = QTime::currentTime();
}

void FullscreenToggle::mouseReleaseEvent(QMouseEvent *event)
{
    this->arbiter.window()->activateWindow();
    this->setWindowOpacity(this->arbiter.system().brightness.value / 510.0);
    QFont f = this->label->font();
    f.setPointSize(f.pointSize() / 1.5);
    this->label->setFont(f);

    if (this->touch_start.msecsTo(QTime::currentTime()) < 100)
        this->arbiter.toggle_fullscreen(false);
}

void FullscreenToggle::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalX() - this->p.x(), event->globalY() - this->p.y());
}
