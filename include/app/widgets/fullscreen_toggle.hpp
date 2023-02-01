#include <QTime>

#include <QDebug>
#include <QDialog>


class Arbiter;

class FullscreenToggle : public QDialog {
    Q_OBJECT

   public:
    FullscreenToggle(Arbiter &arbiter);

    static QWidget *bar(Arbiter &arbiter);

   protected:
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

   private:
    Arbiter &arbiter;
    QPoint p;
    QPoint last_pos;
    QTime touch_start;
    QLabel *label;
};
