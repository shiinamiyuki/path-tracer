//
// Created by xiaoc on 2018/10/8.
//

#ifndef PATH_TRACER_VECTOR3_H
#define PATH_TRACER_VECTOR3_H

#include "util.h"
#include <exception>
struct Vector3 {
    double x, y, z;

    Vector3(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}

    Vector3() : x(0), y(0), z(0) {}

    static inline double dot(const Vector3 &v1, const Vector3 &v2) {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    inline double lengthSqaured() const {
        return dot(*this, *this);
    }

    inline double length() const { return sqrt(lengthSqaured()); }

    static Vector3 cross(const Vector3 &v1, const Vector3 &v2) {
        return {v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x};
    }

    Vector3 &operator+=(const Vector3 &v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    Vector3 &operator-=(const Vector3 &v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    Vector3 &operator*=(const double k) {
        x *= k;
        y *= k;
        z *= k;
        return *this;
    }

    Vector3 mult(const Vector3 &v) {
        return {x * v.x, y * v.y, z * v.z};
    }

    double operator*(const Vector3 &v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    Vector3 operator*(const double t) const {
        return {x * t, y * t, z * t};
    }

    Vector3 operator+(const Vector3 &v) const {
        auto v2 = *this;
        v2 += v;
        return v2;
    }

    Vector3 operator-(const Vector3 &v) const {
        auto v2 = *this;
        v2 -= v;
        return v2;
    }

    void normalize() {
        *this *= 1 / length();
    }

    Vector3 norm() const {
        auto v = *this;
        v.normalize();
        return v;
    }

    Vector3 rotate(const Vector3 &vx, const Vector3 &vy, const Vector3 &vz) const {
        return vx * x + vy * y + vz * z;
    }

    Vector3 rotate(const Vector3 &norm) {
        auto z = Vector3::cross(norm, Vector3{1, 0, 0}).norm();
        auto x = Vector3::cross(norm, z).norm();
        return this->rotate(x, norm, z);
    }
    double get(unsigned int i)const{
        switch (i) {
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
            default:
                throw std::exception();
        }
    }
};
#endif //PATH_TRACER_VECTOR3_H
