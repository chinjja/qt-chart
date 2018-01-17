#ifndef TYPE_H
#define TYPE_H

#include <math.h>

enum Pos {
    TOP, BOTTOM, LEFT, RIGHT
};

template<class InstancePtr, class Type>
inline void set_value(InstancePtr instance, Type &prev, Type &value, bool notify) {
    if(prev != value) {
        prev = value;
        if(notify) instance->fire();
    }
}

#endif // TYPE_H
