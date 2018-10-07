#include <iostream>
#include <cmath>
#include <vector>
#include <random>
#include "lib/lodepng/lodepng.h"
#include "fmt/format.h"
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
    Vector3 norm()const{
        auto v= *this;
        v.normalize();
        return v;
    }
    Vector3 rotate(const Vector3&vx, const Vector3&vy, const Vector3&vz)const{
        return vx * x + vy *y + vz * z;
    }
    Vector3 rotate(const Vector3&norm){
        auto z = Vector3::cross(norm,Vector3{1,0,0}).norm();
        auto x = Vector3::cross(norm, z).norm();
        return this->rotate(x,norm,z);
    }
};

inline int toInt(double x) {
    if (x > 1)
        x = 1;
    if (x < 0)
        x = 0;
    x = x * 255;
    return (int) x;
}

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
enum ReflType{
    REFL,
    DIFF,
    EMIT,
};
struct Material {
    ReflType type;
    Vector3 emittance;
    Vector3 reflectance;

    Material() {}

    Material(ReflType t,const Vector3 &r, const Vector3 &e = {}) {
        type = t;
        reflectance = r;
        emittance = e;
    }
};

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

    RenderObject() {}

    virtual Hit intersect(const Ray &ray) { return {nullptr, -1, {0, 0, 0}}; }

    virtual ~RenderObject() {}
};

class Scene {
    std::vector<RenderObject *> objects;
    unsigned int w, h;
    std::vector<Vector3> screen;
    Vector3 camPos,camDir;
public:
    Scene(int _w, int _h) : w(_w), h(_h) {
        screen.resize(w * h);
        camDir = Vector3{0,0,1}.norm();
    }
    void setCamPos(const Vector3&v){
        camPos = v;
    }
    void setCamDir(const Vector3& v){
        camDir = v;
    }
    void addObject(RenderObject *object) {
        objects.emplace_back(object);
    }

    Hit find(const Ray &ray) {
        RenderObject *object = nullptr;
        double minDist = 99999;
        Vector3 norm;
        for (auto i:objects) {
            auto result = i->intersect(ray);
            double d = result.distance;
            if (d > 0 && minDist > d) {
                minDist = d;
                object = i;
                norm = result.normal;
            }
        }
        return {object, minDist, norm};
    }

    static double frand() {
        static std::mt19937_64 mt;
        static std::uniform_real_distribution<double> distribution(0, 1);
        return distribution(mt);
    }

    static Vector3 randomVectorInHemisphere(const Vector3 &norm) {
        auto theta0 = frand() * 2 * M_PI;
        auto theta1 = acos(1 - 2 * frand());
        auto v = Vector3{
            sin(theta0)*sin(theta1),abs(sin(theta1)),-sin(theta0)*cos(theta1)
        };

        auto z = Vector3::cross(norm,Vector3{1,0,0}).norm();
        auto x = Vector3::cross(norm, z).norm();
        return v.rotate(x,norm,z);
    }
    Vector3 trace(const Ray &ray, int depth) {
        static const int maxDepth = 5;
        if (depth >= maxDepth)
            return {};
        auto hit = find(ray);
        auto object = hit.object;
        if (!object) {
            return {};
        }
        auto &material = object->material;
        auto &emit = material.emittance;
        auto &norm = hit.normal;
        Vector3 r;
        if(object->material.type == REFL)
            r = ray.direction - norm * 2 * (ray.direction * norm);
        else if(object->material.type == DIFF)
            r = randomVectorInHemisphere(norm);
        else
            return emit;
        auto newRay = Ray(ray.getIntersectionPoint(hit.distance), r);
        const double p = 1 / (2 * M_PI);
        const double cos_theta = newRay.direction * norm;
        auto BRDF = material.reflectance * (1 / M_PI);
        auto incoming = trace(newRay, depth + 1);
        auto refl = (BRDF.mult(incoming) * (cos_theta / p));
        auto color = emit + refl;
        return color;
    }

    void render(int spp = 4) {
        const double xmax = 1, ymax = 1;
        double sppSqrt = (double) sqrt(spp);
#pragma omp parallel for schedule(static, 1)
        for (int i = 0; i < w; i++) {
            fmt::print("\r rendering ({0}x{0} spp), {1}%", sppSqrt,100 * (double)i/w);
            for (int j = 0; j < h; j++) {
                Vector3 color;
                for (int sx = 0; sx < sppSqrt; sx++) {
                    for (int sy = 0; sy < sppSqrt; sy++) {
                        double x = (double) (i) / w * xmax * 2 - xmax;
                        double y = (double) (j) / h * ymax * 2 - ymax;
                        x = x + xmax / w / 2 * ((double)sx / sppSqrt * 2 - 1);
                        y = y + ymax / h / 2 * ((double)sy / sppSqrt * 2 - 1);
                        auto dir = Vector3{x, y, 1};//.rotate(camDir);
                        color += trace(Ray(camPos + dir, dir), 0);
                    }
                }
                color *= (1.0 / spp);
                screen[(h - j - 1) * w + i] = (color);
            }
        }
        std::vector<unsigned char> pixelBuffer;
        for (auto &i: screen) {
            pixelBuffer.emplace_back(toInt(i.x));
            pixelBuffer.emplace_back(toInt(i.y));
            pixelBuffer.emplace_back(toInt(i.z));
            pixelBuffer.emplace_back(255);
        }
        auto err = lodepng::encode("out.png", pixelBuffer, w, h);
        if (err) std::cout << "encoder error " << err << ": " << lodepng_error_text(err) << std::endl;
    }
};

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
            if (d < 0) {
                return {nullptr, -1, {}};
            }
            auto i = ray.getIntersectionPoint(d);
            auto norm = i - center;
            return {nullptr, d, norm};
        }
    }
};


void CornellBox() {
    Scene scene(500, 500);
    scene.addObject(new Sphere({-1e5,0,0},Material(DIFF,{1,0,0}),1e5-20)); //left
    scene.addObject(new Sphere({1e5,0,0},Material(DIFF,{0,1,0}),1e5-20)); //right
    scene.addObject(new Sphere({0,-1e5,0},Material(DIFF,{0.5,0.5,0.5}),1e5-10)); //bottom
    scene.addObject(new Sphere({-3, -0.3, 10}, Material(REFL,{1, 1, 1}, {}), 3));
    scene.addObject(new Sphere({1, -0.5, 2}, Material(REFL,{1, 0, 0}, {0, 0, 0}), 0.4));
    scene.addObject(new Sphere({0, 0, 1e5},
                               Material(DIFF,{0.2,0.2,0.2}),
                               1e5 - 20));
    scene.addObject(new Sphere({0, 10000, 0},
                               Material(EMIT,{0, 0, 0}, {12,12,12}),
                               10000 - 200));
    scene.render(100);
}
void wiki(){
    Scene scene(500,500);
    scene.setCamPos({0,1.5,-3});
    scene.addObject(new Sphere({0, 0, 1e5},
                               Material(DIFF,{0.2,0.2,0.2}),
                               1e5 - 20));
    scene.addObject(new Sphere({-10000,0, 0},
                               Material(EMIT,{0, 0, 0}, {1,1,1}),
                               10000 - 200));
    scene.addObject(new Sphere({0,-1e5,0},Material(DIFF,{0.5,0.5,0.5}),1e5)); //bottom
    scene.addObject(new Sphere({-1,3,3},Material(DIFF,{1,1,1}),3));
    scene.addObject(new Sphere({0.6,0.5,0},Material(DIFF,{1,0,0}),0.5));
    scene.render(4900);
}
int main(){
  wiki();
}
