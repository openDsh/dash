#include <QtWidgets>

class MediaTab : public QWidget {
    Q_OBJECT

   public:
    MediaTab(QWidget *parent = 0);

   private:
};

class BluetoothPlayerTab : public QWidget {
    Q_OBJECT

   public:
    BluetoothPlayerTab(QWidget *parent = 0);

private:
    QWidget *track_widget();
    QWidget *controls_widget();
};

class RadioPlayerTab : public QWidget {
    Q_OBJECT

   public:
    RadioPlayerTab(QWidget *parent = 0);
};
