//
// Created by xiaoc on 2018/10/9.
//

#ifndef PATH_TRACER_HIT_H
#define PATH_TRACER_HIT_H

#include "vector3.h"

class RenderObject;

struct Hit {
    RenderObject *object;
    double distance;
    Vector3 normal;

    Hit() { object = nullptr; distance = -1;}

    Hit(RenderObject *_o, double _d, const Vector3 &_n) :
            object(_o), distance(_d), normal(_n) { normal.normalize(); }

    inline bool hit() const { return distance > eps; }
    const Hit& operator || (const Hit& rhs)const{
        if(hit())
            return *this;
        else
            return rhs;
    }
};

#endif //PATH_TRACER_HIT_H
