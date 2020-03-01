#include <QtWidgets>

#include <app/widgets/switch.hpp>

Switch::Switch(QWidget* parent) : QAbstractButton(parent)
{
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
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);

    QColor track_brush = this->isChecked() ? palette().color(QPalette::AlternateBase) : this->track_color;
    QColor thumb_brush = this->isChecked() ? palette().color(QPalette::Base) : this->thumb_color;

    painter.setBrush(track_brush);
    painter.drawRoundedRect(this->margin, this->margin, this->width() - 2 * this->margin,
                            this->height() - 2 * this->margin, this->track_radius, this->track_radius);
    painter.setBrush(thumb_brush);
    painter.drawEllipse(this->offset - this->thumb_radius, this->base_offset - this->thumb_radius,
                        2 * this->thumb_radius, 2 * this->thumb_radius);
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
        QPropertyAnimation *offset_animation = new QPropertyAnimation(this, "offset");
        offset_animation->setDuration(120);
        offset_animation->setStartValue(this->offset);
        offset_animation->setEndValue(this->end_offset(this->isChecked()));
        offset_animation->start();
    }
}

void Switch::nextCheckState()
{
    QAbstractButton::nextCheckState();

    bool checked = this->isChecked();
    this->offset = this->end_offset(checked);
    emit stateChanged(checked);
}
