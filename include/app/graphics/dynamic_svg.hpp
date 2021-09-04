#pragma once

#include <QByteArray>
#include <QColor>
#include <QDomDocument>
#include <QDomElement>
#include <QMap>
#include <QObject>
#include <QSize>
#include <QString>
#include <QStringList>

class DynamicSVG : public QObject {
    Q_OBJECT

   public:
    DynamicSVG(QString path, QObject *parent = nullptr);

    bool rotate(QString id, int16_t degree);
    bool scale(QString id, float factor);
    bool toggle(QString id, bool hide);
    bool fill(QString id, QColor color);
    bool outline(QString id, QColor color);
    bool recolor(QString id, QColor color);
    bool recolor(QColor color, QStringList filter = {});
    bool set_text(QString id, QString text);

    inline QByteArray bytes() { return this->doc.toByteArray(); }
    inline QString attribute(QString id, QString attribute) { return this->elements[id].attribute(attribute); }

   private:
    QDomDocument doc;
    QSize size;

    QMap<QString, QDomElement> elements;

    void add_elements(QDomElement element);
    bool update(QString id, QString text);
    bool update(QString id, QString attribute, QString value);
};
