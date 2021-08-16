#pragma once

#include <QByteArray>
#include <QColor>
#include <QDomDocument>
#include <QDomElement>
#include <QMap>
#include <QObject>
#include <QString>

class DynamicSVG : public QObject {
    Q_OBJECT

   public:
    DynamicSVG(QString path, QObject *parent = nullptr);

    QByteArray bytes() { return this->doc.toByteArray(); }
    QString attribute(QString id, QString attribute) { return this->elements[id].attribute(attribute); }
    void update(QString id, QString text) { this->elements[id].firstChild().setNodeValue(text); }
    void update(QString id, QString attribute, QString value) { this->elements[id].setAttribute(attribute, value); }

    void rotate(QString id, int16_t degree);
    void toggle(QString id, bool hide);
    void fill(QString id, QColor color);
    void outline(QString id, QColor color);
    void recolor(QString id, QColor color);
    void recolor(QColor color);

   private:
    QDomDocument doc;

    QMap<QString, QDomElement> elements;
};
