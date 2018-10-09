//
// Created by xiaoc on 2018/10/8.
//

#ifndef PATH_TRACER_CUBE_H
#define PATH_TRACER_CUBE_H

#include "util.h"
#include "renderobject.h"
class Cube : public RenderObject {
public:
    double size;
    Vector3 min, max;
    Cube(const Vector3 &c, const Material &m, double s) : RenderObject() {
        material = m;
        min = c;
        max = Vector3{c.x + s, c.y + s, c.z + s};
        size = s;
        center = (min + max) * 0.5;
    }

    static int sgn(double x) {
        if (x < 0)
            return -1;
        else
            return 1;
    }

    Hit intersect(const Ray &ray) override {
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
        auto getAngle = [](const Vector3& v, const Vector3& v2){
            return acos(abs(v*v2));
        };
        if (equal(abs(n.x), size / 2)) {
            n.y = n.z = 0;
        }
        else if (equal(abs(n.y), size / 2)) {
            n.z = n.x = 0;
        }
        else if (equal(abs(n.z), size / 2)) {
            n.x = n.y = 0;
        }else{
            return Hit{nullptr,-1,{}};
        }
      //  auto theta = acos(n0.norm() * n.norm()) /M_PI * 180;
        return Hit{this, t, n};
      /*
        static Vector3 x = {1,0,0},y ={0,1,0},z = {0,0,1};
        if(getAngle(x,n0)< M_PI_4){
            return Hit{this,t,x};
        }
        if(getAngle(y,n0)< M_PI_4){
            return Hit{this,t,y};
        }
        if(getAngle(z,n0)< M_PI_4){
            return Hit{this,t,z};
        }
        return Hit{nullptr, -1, {}};*/

    }
};

#endif //PATH_TRACER_CUBE_H
