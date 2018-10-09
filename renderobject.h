//
// Created by xiaoc on 2018/10/8.
//

#ifndef PATH_TRACER_RENDEROBJECT_H
#define PATH_TRACER_RENDEROBJECT_H

#include "ray.h"
#include "material.h"
class RenderObject;
struct Hit {
    RenderObject *object;
    double distance;
    Vector3 normal;

    Hit() { object = nullptr; }

    Hit(RenderObject *_o, double _d, const Vector3 &_n) :
            object(_o), distance(_d), normal(_n) { normal.normalize(); }
};
class RenderObject {

public:
    Material material;
    Vector3 center;
    RenderObject() {}

    virtual Hit intersect(const Ray &ray) { return {nullptr, -1, {0, 0, 0}}; }

    virtual ~RenderObject() {}
};


#endif //PATH_TRACER_RENDEROBJECT_H
