#include <QHBoxLayout>

#include "app/arbiter.hpp"

#include "app/quick_views/quick_view.hpp"

QuickView::QuickView(Arbiter &arbiter, QString pretty_name, QWidget *widget)
    : arbiter(arbiter)
    , pretty_name_(pretty_name)
    , widget_(widget)
{
}

NullQuickView::NullQuickView(Arbiter &arbiter)
    : QFrame()
    , QuickView(arbiter, "none", this)
{
}

void NullQuickView::init()
{
}

VolumeQuickView::VolumeQuickView(Arbiter &arbiter)
    : QFrame()
    , QuickView(arbiter, "volume", this)
{
}

void VolumeQuickView::init()
{
    auto layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(this->arbiter.forge().volume_slider());
}

BrightnessQuickView::BrightnessQuickView(Arbiter &arbiter)
    : QFrame()
    , QuickView(arbiter, "brightness", this)
{
}

void BrightnessQuickView::init()
{
    auto layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(this->arbiter.forge().brightness_slider());
}
