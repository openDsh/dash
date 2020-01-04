#include <QtWidgets>

#include <app/switch.hpp>

Switch::Switch(QWidget* parent) : QAbstractButton(parent)
{
    this->theme = Theme::get_instance();

    QAbstractButton::setCheckable(true);
    QAbstractButton::setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    this->track_radius = 7;
    this->thumb_radius = 10;

    this->margin = std::max(0, this->thumb_radius - this->track_radius);
    this->base_offset = std::max(this->thumb_radius, this->track_radius);
    this->end_offset = [this](bool checked) { return checked ? this->width() - this->base_offset : this->base_offset; };
    this->offset = this->base_offset;
}

QSize Switch::sizeHint() const
{
    return QSize(4.5 * this->track_radius + 2 * this->margin, 2 * this->track_radius + 2 * this->margin);
}

void Switch::paintEvent(QPaintEvent*)
{
    QPainter p(this);

    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);

    // pull these colors from the stylesheet
    auto track_brush = this->isChecked() ? palette().color(QPalette::AlternateBase) : this->track_color;
    auto thumb_brush = this->isChecked() ? palette().color(QPalette::Base) : this->thumb_color;

    p.setBrush(track_brush);
    p.drawRoundedRect(this->margin, this->margin, this->width() - 2 * this->margin, this->height() - 2 * this->margin,
                      this->track_radius, this->track_radius);
    p.setBrush(thumb_brush);
    p.drawEllipse(this->offset - this->thumb_radius, this->base_offset - this->thumb_radius, 2 * this->thumb_radius,
                  2 * this->thumb_radius);
}

void Switch::resizeEvent(QResizeEvent* event)
{
    QAbstractButton::resizeEvent(event);
    this->offset = this->end_offset(this->isChecked());
}

void Switch::mouseReleaseEvent(QMouseEvent* event)
{
    QAbstractButton::mouseReleaseEvent(event);
    if (event->button() == Qt::LeftButton) {
        auto* anim = new QPropertyAnimation(this, "offset");
        anim->setDuration(120);
        anim->setStartValue(this->offset);
        anim->setEndValue(this->end_offset(this->isChecked()));
        anim->start();
    }
}

void Switch::nextCheckState()
{
    QAbstractButton::nextCheckState();
    auto checked = this->isChecked();
    this->offset = this->end_offset(checked);
    emit stateChanged(checked);
}
