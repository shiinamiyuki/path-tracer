//
// Created by xiaoc on 2018/10/8.
//

#ifndef PATH_TRACER_UTIL_H
#define PATH_TRACER_UTIL_H

#include <cmath>
#include <algorithm>
#include <random>
#include <thread>
const double eps = 1e-2;

inline bool equal(double x, double y) {
    return abs(x - y) < 1e-8;
}
inline int toInt(double x) {
    if (x > 1)
        x = 1;
    if (x < 0)
        x = 0;
    x = x * 255;
    return (int) x;
}
inline void ptAssert(bool x){
    if(!x)
    {
    //    throw std::exception();
    }
}
#endif //PATH_TRACER_UTIL_H
