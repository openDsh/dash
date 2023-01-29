#include <QTime>

#include <QDebug>

#include "app/widgets/dialog.hpp"


class Arbiter;

class FullscreenToggle : public Dialog {
    Q_OBJECT

   public:
    FullscreenToggle(Arbiter &arbiter);

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
};
