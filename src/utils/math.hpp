#ifndef MATH_HPP
#define MATH_HPP

inline int int_ceil(double x) {
    int i = (int)x;
    return i + (x > i);
}

inline std::string int2signed_str(int x) {
    std::string res = "";
    if (x > 0) {
        res += "+";
    }
    res += std::to_string(x);
    return res;
}

#endif