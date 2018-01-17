#ifndef AXIS_H
#define AXIS_H

#include <QWidget>
#include <QDebug>

#include "type.h"
#include "range.h"

using namespace std;



class Axis;

class AxisChangeEvent {
public:
    Axis* axis;

public:
    AxisChangeEvent(Axis *_axis) {
        axis = _axis;
    }
};

class AxisChangeListener {
public:
    virtual void onAxisChanged(const AxisChangeEvent* event) = 0;
};

class Axis {
private:
    QString name;
    Range range;
    bool auto_range;
    bool invert;
    vector<AxisChangeListener*> listeners;

public:
    Axis(QString _name, Range _range, bool _invert = false) : name(_name), range(_range), auto_range(false), invert(_invert) {

    }
    Axis(QString _name, qreal min = 0, qreal max = 1, bool invert = false) : Axis(_name, Range(min, max), invert) {

    }
    ~Axis() {
        qDebug() << "axis: " << name << " destroy";
    }
    QString getName() const {
        return name;
    }
    void setName(QString name) {
        this->name = name;
    }
    void setRange(qreal min, qreal max, bool notify = true) {
        setRange(Range(min, max), notify);
    }
    void setRange(Range range, bool notify = true) {
        if(this->range == range) return;
        this->range = range;
        if(notify) fire();
    }
    Range getRange() const {
        return range;
    }
    void setUpper(qreal upper) {
        setRange(getLower(), upper);
    }
    qreal getUpper() const {
        return range.max();
    }
    void setLower(qreal lower) {
        setRange(lower, getUpper());
    }
    qreal getLower() const {
        return range.min();
    }
    void setAutoRange(bool auto_range) {
        if(this->auto_range != auto_range) {
            this->auto_range = auto_range;
            fire();
        }
    }
    bool isAutoRange() const {
        return auto_range;
    }
    void setInvert(bool invert) {
        if(this->invert != invert) {
            this->invert = invert;
            fire();
        }
    }
    bool isInvert() const {
        return invert;
    }
    qreal point_to_value(qreal point, QRectF area, Pos pos) const {
        qreal axis_min = range.min();
        qreal axis_max = range.max();

        qreal area_min;
        qreal area_max;

        bool ivt = isInvert();

        switch(pos) {
        case TOP:
        case BOTTOM:
            area_min = area.x();
            area_max = area.x() + area.width();
            break;
        case LEFT:
        case RIGHT:
            ivt = !ivt;
            area_min = area.y();
            area_max = area.y() + area.height();
            break;
        }
        qreal axis_delta = axis_max - axis_min;
        qreal area_delta = area_max - area_min;
        qreal rate = axis_delta / area_delta;
        if(ivt) {
            return (area_max - point) * rate + axis_min;
        } else {
            return (point - area_min) * rate + axis_min;
        }
    }
    qreal value_to_point(qreal value, QRectF area, Pos pos) const {
        qreal axis_min = range.min();
        qreal axis_max = range.max();

        qreal area_min = 0;
        qreal area_max = 0;

        bool ivt = isInvert();

        switch(pos) {
        case TOP:
        case BOTTOM:
            area_min = area.x();
            area_max = area.x() + area.width();
            break;
        case LEFT:
        case RIGHT:
            ivt = !ivt;
            area_min = area.y();
            area_max = area.y() + area.height();
            break;
        }
        qreal axis_delta = axis_max - axis_min;
        qreal area_delta = area_max - area_min;
        qreal rate =  area_delta / axis_delta;

        if(ivt) {
            return (axis_max - value) * rate + area_min;
        } else {
            return (value - axis_min) * rate + area_min;
        }
    }

    void addAxisChangeListener(AxisChangeListener* listener) {
        listeners.push_back(listener);
    }
    void removeAxisChangeListener(AxisChangeListener* listener) {
        listeners.erase(find(listeners.begin(), listeners.end(), listener));
    }
protected:
    void fire() {
        AxisChangeEvent event(this);
        for(AxisChangeListener* l : listeners) {
            l->onAxisChanged(&event);
        }
    }
};

#endif // AXIS_H
