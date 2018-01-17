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
    bool equals(Range& other) const {
        return _min == other._min && _max == other._max;
    }
    bool operator == (Range& other) {
        return equals(other);
    }
    bool operator != (Range& other) {
        return !equals(other);
    }
    Range operator *(qreal rate) {
        qreal half = delta()/2;
        qreal cent = _min + half;
        half *= rate;
        return Range(cent-half, cent+half);
    }
};

#endif // RANGE_H
