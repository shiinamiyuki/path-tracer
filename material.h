//
// Created by xiaoc on 2018/10/8.
//

#ifndef PATH_TRACER_MATERIAL_H
#define PATH_TRACER_MATERIAL_H

#include "vector3.h"
enum ReflType {
    SPEC,
    DIFF,
    EMIT,
};
struct Material {
    ReflType type;
    Vector3 emittance;
    Vector3 reflectance;

    Material() {}

    Material(ReflType t, const Vector3 &r, const Vector3 &e = {}) {
        type = t;
        reflectance = r;
        emittance = e;
    }
};
#endif //PATH_TRACER_MATERIAL_H
