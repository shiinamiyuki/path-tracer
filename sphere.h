//
// Created by xiaoc on 2018/10/8.
//

#ifndef PATH_TRACER_SPHERE_H
#define PATH_TRACER_SPHERE_H

#include "util.h"
#include "renderobject.h"
class Sphere : public RenderObject {
public:
    double radius;
    Vector3 center;

    Sphere(const Vector3 &c, const Material &m, double r) : RenderObject() {
        material = m;
        radius = r;
        center = c;
    }

    Hit intersect(const Ray &ray) override {
        auto a = 1;
        auto b = 2 * (ray.direction * (ray.origin - center));
        auto c = (ray.origin - center).lengthSqaured() - radius * radius;
        auto delta = b * b - 4 * a * c;
        if (delta < 0)
            return {nullptr, -1, {}};
        else {
            delta = sqrt(delta);
            auto d1 = (-b + delta) / (2 * a);
            auto d2 = (-b - delta) / (2 * a);
            double d;
            if (d1 < 0)
                d = d2;
            else if (d2 < 0)
                d = d1;
            else {
                d = std::min(d1, d2);
            }
            if (d < eps) {
                return {nullptr, -1, {}};
            }
            auto i = ray.getIntersectionPoint(d);
            auto norm = i - center;
            norm.normalize();
            if (norm * ray.direction > 0) {
                //            throw std::runtime_error("");
            }
            return {nullptr, d, norm};
        }
    }
};

#endif //PATH_TRACER_SPHERE_H
