#ifndef SWITCH_HPP_
#define SWITCH_HPP_

#include <QtWidgets>

class Switch : public QAbstractButton {
    Q_OBJECT
    Q_PROPERTY(QColor track_color READ get_track_color WRITE set_track_color)
    Q_PROPERTY(QColor thumb_color READ get_thumb_color WRITE set_thumb_color)
    Q_PROPERTY(int offset READ get_offset WRITE set_offset)

   public:
    Switch(QWidget *parent = nullptr);

    QSize sizeHint() const override;

    QColor get_track_color() { return this->track_color; };
    QColor get_thumb_color() { return this->thumb_color; };
    int get_offset() { return this->offset; }

    void set_track_color(QColor track_color) { this->track_color = track_color; }
    void set_thumb_color(QColor thumb_color) { this->thumb_color = thumb_color; }
    void set_offset(int offset) { this->offset = offset; }
    void scale(double scale)
    {
        this->track_radius = std::ceil(BASE_TRACK_RADIUS * scale);
        this->thumb_radius = std::ceil(BASE_THUMB_RADIUS * scale);
        this->update_properties();
    }

   protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void checkStateSet() override;
    void nextCheckState() override;

   private:
    const int BASE_TRACK_RADIUS = 8;
    const int BASE_THUMB_RADIUS = 12;

    int track_radius;
    int thumb_radius;

    QColor track_color = Qt::transparent;
    QColor thumb_color = Qt::transparent;
    int offset;
    int margin;
    int base_offset;
    std::function<int(bool)> end_offset;

    void update_properties();

   signals:
    void stateChanged(bool state);
};

#endif
