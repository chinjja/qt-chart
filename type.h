#ifndef TYPE_H
#define TYPE_H

#include <math.h>

enum Pos {
    TOP, BOTTOM, LEFT, RIGHT
};

class Insets {
public:
    double left;
    double right;
    double top;
    double bottom;

public:
    Insets(double _left = 0, double _right = 0, double _top = 0, double _bottom = 0)
    : left(_left), right(_right), top(_top), bottom(_bottom) {}
};


#endif // TYPE_H
