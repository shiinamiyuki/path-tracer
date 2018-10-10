//
// Created by xiaoc on 2018/10/8.
//

#ifndef PATH_TRACER_CUBE_H
#define PATH_TRACER_CUBE_H

#include "util.h"
#include "renderobject.h"
#include "aabb.h"
class Cube : public RenderObject {
public:
    AABB box;
    Cube(){}
    Cube(const Vector3 &c, const Material &m, double s) : RenderObject(),box(c,s) {
        material = m;
        center = box.center ;
    }

    static int sgn(double x) {
        if (x < 0)
            return -1;
        else
            return 1;
    }

    Hit intersect(const Ray &ray) override {
       return box.intersect(ray);
    }
};

#endif //PATH_TRACER_CUBE_H
