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
#include "kdtree.h"

class PathTracerWindow;
class Scene {
    std::vector<RenderObject *> objects;
    std::vector<RenderObject*> lightSources;
    KDTree * kdTree;
    unsigned int w, h;
    std::vector<Vector3> screen;
    Vector3 camPos, camDir;
    double sx, sy;
    double sampleCount;
    bool done;
    KDTree* constructKDTree(const AABB& box,const std::vector<RenderObject*>&,unsigned int depth = 0);
public:
    friend class PathTracerWindow;
    std::vector<unsigned char> pixelBuffer;

    Scene(int _w, int _h) : w(_w), h(_h) {
        screen.resize(w * h);
        camDir = Vector3{0, 0, 1}.norm();
        sx = sy = 0;
        sampleCount = 0;
        done = false;
        kdTree = nullptr;
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
    Hit find0(const Ray &ray){
        return kdTree->search(ray);
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
                if(result.object){
                    object = result.object;
                }else
                    object = i;
                norm = result.normal;
            }
        }
        return {object, minDist, norm};
    }
    RenderObject *findALightSource();

    Vector3 randomVectorInHemisphere(const Vector3 &norm, unsigned short *Xi) {

        auto theta0 = erand48(Xi) * 2 * M_PI;
        auto theta1 = acos(1 - 2 * erand48(Xi));
        auto v = Vector3{
                sin(theta0) * sin(theta1), abs(sin(theta1)), -sin(theta0) * cos(theta1)
        };
        //auto v = Vector3(erand48(Xi) * 2-1,erand48(Xi),erand48(Xi) * 2-1).norm();

        auto z = Vector3::cross(norm, abs(norm.y) > 0.1 ? Vector3{1, 0, 0} : Vector3(0, 1, 0)).norm();
        auto x = Vector3::cross(norm, z).norm();
        v = v.rotate(x, norm, z);
        if (v * norm < 0) {
            throw std::runtime_error("");
        }
        return v;
    }
    void trace(int i,int j, double spp,unsigned short * Xi);
    void save();

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

    Vector3 trace(const Ray &ray, int depth, unsigned short *Xi);

    void renderSamples(int spp = 4, int samples = 1);

    void render(int spp, int s = 4) {
        spp = sqrt(spp);
        spp *= spp;
        while (!done) {
            renderSamples(spp, s);
        }
    }
    void prepare();
    void addLightSource(RenderObject*o){
        lightSources.emplace_back(o);
    }
};

#endif //PATH_TRACER_SCENE_H
