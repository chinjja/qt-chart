#ifndef RANGE_H
#define RANGE_H

#include <QtCore>

class Range {
private:
    qreal _min;
    qreal _max;

public:
    Range(qreal min, qreal max)
        : _min(min), _max(max) {
        if(_min > _max) throw 1;
    }
    qreal min() const {
        return _min;
    }
    qreal max() const {
        return _max;
    }
    qreal delta() const {
        return _max - _min;
    }
    bool inside(qreal v) const {
        return v >= _min && v <= _max;
    }
    bool outside(qreal v) const {
        return !inside(v);
    }
    bool operator == (Range& other) {
        return _min == other._min && _max == other._max;
    }
    Range operator *(qreal rate) {
        return Range(_min * rate, _max * rate);
    }
};

#endif // RANGE_H
