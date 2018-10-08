//
// Created by xiaoc on 2018/10/8.
//

#ifndef PATH_TRACER_RAY_H
#define PATH_TRACER_RAY_H

#include "vector3.h"
struct Ray {
    Vector3 origin, direction;

    Ray(const Vector3 &_origin, const Vector3 &_direction)
            : origin(_origin), direction(_direction) {
        direction.normalize();
    }

    Vector3 getIntersectionPoint(double t) const {
        return direction * t + origin;
    }
};

#endif //PATH_TRACER_RAY_H
