#include <QIconEngine>

class Arbiter;

class IconEngine : public QIconEngine
{
   public:
    IconEngine(Arbiter &arbiter, QString icon, bool colorize);
    virtual void init_painter(QPainter *painter, QIcon::Mode mode, QIcon::State state) const;
    virtual void addFile(const QString &file, const QSize &size, QIcon::Mode mode, QIcon::State state) override;
    void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) override;
    virtual QIconEngine *clone() const override;
    QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) override;

   protected:
    Arbiter &arbiter;
    QString icon;
    QString alt_icon;
    bool colorize;
};

class StylizedIconEngine : public IconEngine
{
   public:
    StylizedIconEngine(Arbiter &arbiter, QString icon, bool colorize);
    void init_painter(QPainter *painter, QIcon::Mode mode, QIcon::State state) const override;
    void addFile(const QString &file, const QSize &size, QIcon::Mode mode, QIcon::State state) override;
    QIconEngine *clone() const override;

   private:
    bool stylized;
};
