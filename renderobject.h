//
// Created by xiaoc on 2018/10/8.
//

#ifndef PATH_TRACER_RENDEROBJECT_H
#define PATH_TRACER_RENDEROBJECT_H

#include "ray.h"
#include "material.h"
#include "hit.h"
class RenderObject {

public:
    Material material;
    Vector3 center;
    RenderObject() {}

    virtual Hit intersect(const Ray &ray) { return {nullptr, -1, {0, 0, 0}}; }
    virtual void prepare(){}
    virtual ~RenderObject() {}
};


#endif //PATH_TRACER_RENDEROBJECT_H
