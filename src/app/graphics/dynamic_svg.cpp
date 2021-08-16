#include <sstream>

#include <QFile>
#include <QPointF>

#include "app/graphics/dynamic_svg.hpp"

DynamicSVG::DynamicSVG(QString path, QObject *parent)
    : QObject(parent)
    , doc()
{
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        this->doc.setContent(&file);

        auto paths = this->doc.documentElement().childNodes();
        for (int i = 0; i < paths.length(); i++) {
            auto element = paths.item(i).toElement();
            if (element.hasAttribute("id"))
                elements.insert(element.attribute("id"), element);
        }
    }
}

void DynamicSVG::rotate(QString id, int16_t degree)
{
    QString cmds = this->attribute(id, "d");
    cmds.remove(0, cmds.indexOf('M') + 1);

    char _;
    QPointF point;
    std::stringstream ss(cmds.toStdString());
    ss >> point.rx() >> _ >> point.ry();

    this->update(id, "transform", QString("rotate(%1 %2 %3)").arg(degree).arg(point.x()).arg(point.y()));
}

void DynamicSVG::toggle(QString id, bool toggle)
{
    this->update(id, "visibility", toggle ? "visible" : "hidden");
}

void DynamicSVG::fill(QString id, QColor color)
{
    this->update(id, "fill", color.isValid() ? color.name() : "none");
}

void DynamicSVG::outline(QString id, QColor color)
{
    this->update(id, "stroke", color.name());
}

void DynamicSVG::recolor(QString id, QColor color)
{
    if (this->elements[id].hasAttribute("stroke"))
        this->outline(id, color);
    else if (this->elements[id].hasAttribute("fill"))
        this->fill(id, color);
}

void DynamicSVG::recolor(QColor color)
{
    for (auto id : this->elements.keys())
        this->recolor(id, color);
}
