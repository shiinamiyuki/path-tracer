//
// Created by xiaoc on 2018/10/9.
//

#ifndef PATH_TRACER_AABB_H
#define PATH_TRACER_AABB_H

#include "util.h"
#include "vector3.h"
#include "ray.h"
#include "hit.h"

auto le = [](const Vector3 &min, const Vector3 &max) {
    return min.x - eps <= max.x && min.y - eps <= max.y && min.z - eps <= max.z;
};
auto lt = [](const Vector3 &min, const Vector3 &max) {
    return min.x - eps < max.x && min.y - eps < max.y && min.z - eps < max.z;
};

class AABB {
public:
    Vector3 min, max;
    Vector3 center;
    double sizeX, sizeY, sizeZ;

    AABB() {}

    AABB(const Vector3 &_min, const Vector3 &_max) : min(_min), max(_max) {
        if (min.x > max.x) {
            std::swap(min, max);
        }
        center = (min + max) * 0.5;
        sizeX = -min.x + max.x;
        sizeY = -min.y + max.y;
        sizeZ = -min.z + max.z;
    }

    AABB(const Vector3 &c, double s) {
        min = c;
        max = Vector3{c.x + s, c.y + s, c.z + s};
        sizeX = sizeY = sizeZ = s;
        center = (min + max) * 0.5;
    }

    Hit intersect(const Ray &ray) {
        double tmin = (min.x - ray.origin.x) / ray.direction.x;
        double tmax = (max.x - ray.origin.x) / ray.direction.x;
        if (tmin > tmax)
            std::swap(tmin, tmax);
        double tymin = (min.y - ray.origin.y) / ray.direction.y;
        double tymax = (max.y - ray.origin.y) / ray.direction.y;
        if (tymin > tymax)
            std::swap(tymin, tymax);
        if (tmin > tymax || tymin > tmax)
            return Hit{nullptr, -1, {}};

        if (tymin > tmin)
            tmin = tymin;

        if (tymax < tmax)
            tmax = tymax;
        double tzmin = (min.z - ray.origin.z) / ray.direction.z;
        double tzmax = (max.z - ray.origin.z) / ray.direction.z;

        if (tzmin > tzmax) std::swap(tzmin, tzmax);
        if (tmin > tzmax || tzmin > tmax)
            return Hit{nullptr, -1, {}};

        if (tzmin > tmin)
            tmin = tzmin;

        if (tzmax < tmax)
            tmax = tzmax;

        double t;
        if (tmin < eps)
            t = tmax;
        else if (tmax < eps) {
            t = tmin;
        } else {
            t = std::min(tmin, tmax);
        }
        if (t <= eps)
            return Hit{nullptr, -1, {}};
        auto p = ray.getIntersectionPoint(t);
        auto n = p - center;
        auto n0 = n.norm();
        if (equal(fabs(n.x), sizeX / 2)) {
            n.y = n.z = 0;
        } else if (equal(fabs(n.y), sizeY / 2)) {
            n.z = n.x = 0;
        } else if (equal(fabs(n.z), sizeZ / 2)) {
            n.x = n.y = 0;
        } else {
            return Hit{nullptr, -1, {}};
        }
        return Hit{nullptr, t, n};

    }

    bool contains(const AABB &box) const {

        bool a = le(min, box.min);
        bool b = le(box.max, max);
        return a && b;
    }

    bool contains(const Vector3 &v) const {
        return le(min, v) && le(v, max);
    }

    void assetContains(const AABB &box) const {
        //   ptAssert(contains(box));
    }
};

#endif //PATH_TRACER_AABB_H
