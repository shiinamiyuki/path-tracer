//
// Created by xiaoc on 2018/10/8.
//

#ifndef PATH_TRACER_SCENE_H
#define PATH_TRACER_SCENE_H

#include <vector>
#include <exception>

#include <iostream>
#include "renderobject.h"
#include "util.h"
#include "lib/rand48/erand48.h"
#include "fmt/format.h"
#include "lib/lodepng/lodepng.h"

class Scene {
    std::vector<RenderObject *> objects;
    unsigned int w, h;
    std::vector<Vector3> screen;
    Vector3 camPos, camDir;
    double sx, sy;
    double sampleCount;
    bool done;
public:
    std::vector<unsigned char> pixelBuffer;

    Scene(int _w, int _h) : w(_w), h(_h) {
        screen.resize(w * h);
        camDir = Vector3{0, 0, 1}.norm();
        sx = sy = 0;
        sampleCount = 0;
        done = false;
    }

    void setCamPos(const Vector3 &v) {
        camPos = v;
    }

    void setCamDir(const Vector3 &v) {
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
            if (d > eps && minDist > d) {
                minDist = d;
                object = i;
                norm = result.normal;
            }
        }
        return {object, minDist, norm};
    }

    Vector3 randomVectorInHemisphere(const Vector3 &norm, unsigned short *Xi) {

        auto theta0 = erand48(Xi) * 2 * M_PI;
        auto theta1 = acos(1 - 2 * erand48(Xi));
        auto v = Vector3{
                sin(theta0) * sin(theta1), abs(sin(theta1)), -sin(theta0) * cos(theta1)
        };
        //auto v = Vector3(erand48(Xi) * 2-1,erand48(Xi),erand48(Xi) * 2-1).norm();

        auto z = Vector3::cross(norm, abs(norm.y)>0.1 ?Vector3{1, 0, 0}:Vector3(0,1,0)).norm();
        auto x = Vector3::cross(norm, z).norm();
        v = v.rotate(x, norm, z);
        if (v * norm < 0) {
            throw std::runtime_error("");
        }
        return v;
    }

    Vector3 trace(const Ray &ray, int depth, unsigned short *Xi) {
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
        if (object->material.type == SPEC) {
            double roughness = 0;
            r = ray.direction - norm * 2 * (ray.direction * norm);
            r += Vector3(erand48(Xi) - 0.5, erand48(Xi) - 0.5, erand48(Xi) - 0.5) * roughness;
            r.normalize();
        } else if (object->material.type == DIFF) {
            if (norm * ray.direction > 0)
                norm *= -1;
          /*  double r1 = 2 * M_PI * erand48(Xi), r2 = erand48(Xi), r2s = sqrt(r2);
            auto w = norm;
            auto u = Vector3::cross((abs(w.x) > 0.1) ? Vector3{0, 1, 0} : Vector3{1, 0, 0}, w).norm();
            auto v = Vector3::cross(w, u);
            r = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).norm();*/
            r = randomVectorInHemisphere(norm,Xi);
        } else
            return emit;
        auto newRay = Ray(ray.getIntersectionPoint(hit.distance), r);
        const double p = 1 / (2 * M_PI);
        const double cos_theta = newRay.direction * norm;
        auto BRDF = material.reflectance * (1 / M_PI);
        auto incoming = trace(newRay, depth + 1, Xi);
        auto refl = (BRDF.mult(incoming) * (cos_theta / p));
        auto color = emit + refl;
        return color;
    }

#if 0
    void render(int spp = 4) {
        const double xmax = 1, ymax = 1;
        double sppSqrt = (double) sqrt(spp);
#pragma omp parallel for schedule(static, 1)
        for (int i = 0; i < w; i++) {
            fmt::print("\r rendering ({0}x{0} spp), {1}%", sppSqrt, 100 * (double) i / w);
            for (int j = 0; j < h; j++) {
                Vector3 color;
                for (int sx = 0; sx < sppSqrt; sx++) {
                    for (int sy = 0; sy < sppSqrt; sy++) {
                        double x = (double) (i) / w * xmax * 2 - xmax;
                        double y = (double) (j) / h * ymax * 2 - ymax;
                        x = x + xmax / w / 2 * ((double) sx / sppSqrt * 2 - 1);
                        y = y + ymax / h / 2 * ((double) sy / sppSqrt * 2 - 1);
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

#endif

    void renderSamples(int spp = 4, int samples = 1) {
        const double xmax = 1, ymax = 1;
        const double sppSqrt = (double) sqrt(spp);
        for (int i = 0; i < samples && !done; i++) {
            unsigned short Xi[3] = {0, 0, (unsigned short) i * i * i};
#pragma omp parallel for schedule(static, 1)
            for (int i = 0; i < w; i++) {
                for (int j = 0; j < h; j++) {
                    Vector3 color = screen[(j) * w + i] * sampleCount;
                    double x = (double) (i) / w * xmax * 2 - xmax;
                    double y = (double) (j) / h * ymax * 2 - ymax;
                    x = x + xmax / w / 2 * (sx / sppSqrt * 2 - 1);
                    y = y + ymax / h / 2 * (sy / sppSqrt * 2 - 1);
                    auto dir = Vector3{x, y, 1};//.rotate(camDir);
                    color += trace(Ray(camPos + dir, dir), 0, Xi);
                    screen[(j) * w + i] = (color) * (1 / (sampleCount + 1));
                }
            }
            sy += 1;
            if (sy >= sppSqrt) {
                sx += 1;
                sy = 0;
            }
            if (sx >= sppSqrt) {
                done = true;
                break;
            }

            sampleCount += 1;
        }
        pixelBuffer.clear();
        for (auto &i: screen) {
            pixelBuffer.emplace_back(toInt(i.x));
            pixelBuffer.emplace_back(toInt(i.y));
            pixelBuffer.emplace_back(toInt(i.z));
            pixelBuffer.emplace_back(255);
        }
    }

    void render(int spp, int s = 4) {
        spp = sqrt(spp);
        spp *= spp;
        while (!done) {
            renderSamples(spp, s);
        }
    }
};

#endif //PATH_TRACER_SCENE_H
