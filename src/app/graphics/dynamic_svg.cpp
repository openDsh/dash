#include <sstream>

#include <QFile>
#include <QRegularExpression>

#include "app/graphics/dynamic_svg.hpp"

DynamicSVG::DynamicSVG(QString path, QObject *parent)
    : QObject(parent)
    , doc()
    , size()
{
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        this->doc.setContent(&file);

        auto element = this->doc.documentElement();
        this->size.setWidth(element.attribute("width").toInt());
        this->size.setHeight(element.attribute("height").toInt());

        this->add_elements(element.firstChildElement());
    }
}

bool DynamicSVG::rotate(QString id, int16_t degree)
{
    auto transform = this->attribute(id, "transform");
    transform.replace(QRegularExpression("rotate\\(-?\\d+"), QString("rotate(%1").arg(degree));
    return this->update(id, "transform", transform);
}

bool DynamicSVG::scale(QString id, float factor)
{
    auto transform = this->attribute(id, "transform");
    auto transx = (1 - factor) * (this->size.width() / 2);
    auto transy = (1 - factor) * (this->size.height() / 2);
    transform.replace(QRegularExpression("translate\\(-?\\d+ -?\\d+"), QString("translate(%1 %2").arg(transx).arg(transy));
    transform.replace(QRegularExpression("scale\\(\\d+"), QString("scale(%1").arg(factor));
    return this->update(id, "transform", transform);
}

bool DynamicSVG::toggle(QString id, bool toggle)
{
    return this->update(id, "visibility", toggle ? "visible" : "hidden");
}

bool DynamicSVG::fill(QString id, QColor color)
{
    return this->update(id, "fill", color.isValid() ? color.name() : "none");
}

bool DynamicSVG::outline(QString id, QColor color)
{
    return this->update(id, "stroke", color.name());
}

bool DynamicSVG::recolor(QString id, QColor color)
{
    if (this->elements[id].hasAttribute("stroke"))
        return this->outline(id, color);
    else if (this->elements[id].hasAttribute("fill"))
        return this->fill(id, color);

    return false;
}

bool DynamicSVG::recolor(QColor color, QStringList filter)
{
    bool changed = false;

    for (auto id : this->elements.keys()) {
        if (!filter.contains(id))
            changed |= this->recolor(id, color);
    }

    return changed;
}

bool DynamicSVG::set_text(QString id, QString text)
{
    return this->update(id, text);
}

void DynamicSVG::add_elements(QDomElement element)
{
    if (element.hasAttribute("id"))
        this->elements.insert(element.attribute("id"), element);

    auto child = element.firstChildElement();
    while (!child.isNull()) {
        this->add_elements(child);
        child = child.nextSiblingElement();
    }
}

bool DynamicSVG::update(QString id, QString text)
{
    auto node = this->elements[id].firstChild();
    if (node.nodeValue() == text)
        return false;
    
    node.setNodeValue(text);
    return true;
}
bool DynamicSVG::update(QString id, QString attribute, QString value)
{
    if (this->attribute(id, attribute) == value)
        return false;

    this->elements[id].setAttribute(attribute, value);
    return true;
}
