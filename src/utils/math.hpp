#ifndef MATH_HPP
#define MATH_HPP

inline int int_ceil(double x) {
    int i = (int)x;
    return i + (x > i);
}

#endif