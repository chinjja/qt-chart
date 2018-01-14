#ifndef AXIS_H
#define AXIS_H

#include <QWidget>

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
    Range range;
    bool auto_range;
    bool invert;
    vector<AxisChangeListener*> listeners;

public:
    Axis(Range _range, bool _invert = false) : range(_range), auto_range(false), invert(_invert) {

    }
    Axis(double min, double max, bool invert = false) : Axis(Range(min, max), invert) {

    }
    void setRange(double min, double max, bool notify = true) {
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
    void setUpper(double upper) {
        setRange(getLower(), upper);
    }
    double getUpper() const {
        return range.max();
    }
    void setLower(double lower) {
        setRange(lower, getUpper());
    }
    double getLower() const {
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
    double point_to_value(double point, QRect area, Pos pos) const {
        double axis_min = range.min();
        double axis_max = range.max();

        double area_min;
        double area_max;
        switch(pos) {
        case TOP:
        case BOTTOM:
            area_min = area.x();
            area_max = area.x() + area.width();
            break;
        case LEFT:
        case RIGHT:
            area_min = area.y();
            area_max = area.y() + area.height();
            break;
        }
        double axis_delta = axis_max - axis_min;
        double area_delta = area_max - area_min;
        double rate = axis_delta / area_delta;
        if(isInvert()) {
            return (area_max - point) * rate + axis_min;
        } else {
            return (point - area_min) * rate + axis_min;
        }
    }
    double value_to_point(double value, QRect area, Pos pos) const {
        double axis_min = range.min();
        double axis_max = range.max();

        double area_min = 0;
        double area_max = 0;
        switch(pos) {
        case TOP:
        case BOTTOM:
            area_min = area.x();
            area_max = area.x() + area.width();
            break;
        case LEFT:
        case RIGHT:
            area_min = area.y();
            area_max = area.y() + area.height();
            break;
        }
        double axis_delta = axis_max - axis_min;
        double area_delta = area_max - area_min;
        double rate =  area_delta / axis_delta;

        if(isInvert()) {
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
