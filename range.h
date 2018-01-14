#ifndef RANGE_H
#define RANGE_H


class Range {
private:
    double _min;
    double _max;

public:
    Range(double min, double max)
        : _min(min), _max(max) {
        if(_min > _max) throw 1;
    }
    double min() const {
        return _min;
    }
    double max() const {
        return _max;
    }
    double delta() const {
        return _max - _min;
    }
    bool inside(double v) const {
        return v >= _min && v <= _max;
    }
    bool outside(double v) const {
        return !inside(v);
    }
    bool operator == (Range& other) {
        return _min == other._min && _max == other._max;
    }
};

#endif // RANGE_H
