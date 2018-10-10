//
// Created by xiaoc on 2018/10/9.
//

#include "scene.h"

void Scene::trace(int i, int j, double spp, unsigned short *Xi) {
    const double xmax = 1, ymax = 1;
    Vector3 color = screen[(j) * w + i] * sampleCount;
    double x = (double) (i) / w * xmax * 2 - xmax;
    double y = (double) (j) / h * ymax * 2 - ymax;
    x = x + xmax / w / 2 * (sx / spp * 2 - 1);
    y = y + ymax / h / 2 * (sy / spp * 2 - 1);
    auto dir = Vector3{x, y, 1};//.rotate(camDir);
    color += trace(Ray(camPos + dir, dir), 0, Xi);
    screen[(j) * w + i] = (color) * (1 / (sampleCount + 1));
}

void Scene::renderSamples(int spp, int samples) {

    const double sppSqrt = (double) sqrt(spp);
    if (done)
        return;
    for (int i = 0; i < samples && !done; i++) {
        unsigned short Xi[3] = {0, 0, (unsigned short) i * i * i};
#pragma omp parallel for schedule(static, 1)
        for (int i = 0; i < w; i++) {
            for (int j = 0; j < h; j++) {
                trace(i, j, sppSqrt, Xi);
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

static double frand() {
    static std::random_device seeder;
    static std::mt19937 engine(seeder());
    static std::uniform_real_distribution<double> dist(1, 6);

    return dist(engine);
}

static Vector3 randomVector(const Vector3 &norm, unsigned short *Xi) {
    double r1 = 2 * M_PI * erand48(Xi), r2 = erand48(Xi), r2s = sqrt(r2);
    auto w = norm;
    auto u = Vector3::cross((abs(w.x) > 0.1) ? Vector3{0, 1, 0} : Vector3{1, 0, 0}, w).norm();
    auto v = Vector3::cross(w, u);
    auto r = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).norm();
    return r;
}

static Vector3 randomVectorInCone(const Vector3 &norm, double rad, unsigned short *Xi) {
    double r1 = 2 * M_PI * erand48(Xi), r2 = erand48(Xi) * rad, r2s = sqrt(r2);
    auto w = norm;
    auto u = Vector3::cross((abs(w.x) > 0.1) ? Vector3{0, 1, 0} : Vector3{1, 0, 0}, w).norm();
    auto v = Vector3::cross(w, u);
    auto r = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).norm();
    return r;
}

Vector3 Scene::trace(const Ray &ray, int depth, unsigned short *Xi) {
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
    const double p = 1 / (2 * M_PI);
    auto BRDF = material.reflectance * (1 / M_PI);
    Vector3 color = {0, 0, 0};
    Vector3 r;
    if (object->material.type == SPEC) {
        double roughness = 0;
        r = ray.direction - norm * 2 * (ray.direction * norm);
        r += Vector3(erand48(Xi) - 0.5, erand48(Xi) - 0.5, erand48(Xi) - 0.5) * roughness;
        r.normalize();
    } else if (object->material.type == DIFF) {
        if (norm * ray.direction > 0)
            norm *= -1;
        r = randomVector(norm, Xi);
        auto light = findALightSource();
        auto prob = 0.8;
        auto rad = 0.04; // shooting rays toward the light source
        if (light && (1 + erand48(Xi)) / 2 < prob) {
            auto pt = ray.getIntersectionPoint(hit.distance);
            auto n = randomVectorInCone((light->center - pt).norm(), rad, Xi);
            auto dot = n * norm;
            if (dot > 0) {// only consider rays going out of the surface
                auto r2 = Ray(pt, n);
                auto tr = trace(r2, maxDepth - 1, Xi);
                auto inc = (BRDF.mult(tr)) * (dot / p) * rad;
                color += inc;
            }
        }
    } else
        return emit;
    auto newRay = Ray(ray.getIntersectionPoint(hit.distance), r);
    const double cos_theta = newRay.direction * norm;
    auto incoming = trace(newRay, depth + 1, Xi);
    auto refl = (BRDF.mult(incoming) * (cos_theta / p));
    color += emit + refl;
    return color;
}

RenderObject *Scene::findALightSource() {
    static bool first = true;
    static int cnt = 0;
    if (first) {
        for (auto i:objects) {
            if (i->material.type == EMIT) {
                lightSources.push_back(i);
            }
        }
        first = false;
    }
    if (lightSources.empty())
        return nullptr;
    else {
        if (cnt >= lightSources.size())
            cnt = 0;
        return lightSources[cnt++];
    }
}

void Scene::save() {
    pixelBuffer.clear();
    for (int i = w - 1; i >= 0; i--)
        for (int j = h - 1; j >= 0; j--) {
            auto x = i;
            auto y = h - 1 - j;
            auto v = screen[x * h + y];
            pixelBuffer.emplace_back(toInt(v.x));
            pixelBuffer.emplace_back(toInt(v.y));
            pixelBuffer.emplace_back(toInt(v.z));
            pixelBuffer.emplace_back(255);
        }
}

KDTree *Scene::constructKDTree(const AABB &box, const std::vector<RenderObject *> &vec, unsigned int depth) {

    if (vec.empty())
        return nullptr;
    unsigned int axis = depth % 3;
    if (vec.size() == 1) {
        return new KDTree(vec[0], axis, box);
    }
    auto i = rand() % vec.size();//random pick
    auto median = vec[i];
    auto med = median->center.get(axis);
    auto tree = new KDTree(nullptr, axis, box);
    AABB left, right;
    auto get = [=](unsigned int i) {
        if (i == axis) {
            return med;
        } else {
            return box.max.get(i);
        }
    };
    auto pivot = Vector3{get(0), get(1), get(2)};
    left = AABB(box.min, pivot);
    right = AABB(pivot, box.max);
    std::vector<RenderObject *> v1, v2;
    for (auto idx = 0; idx < vec.size(); idx++) {
        if (vec[idx]->center.get(axis) < med) {
            v1.emplace_back(vec[idx]);
        } else {
            v2.emplace_back(vec[idx]);
        }
    }
    tree->addLeft(constructKDTree(left, v1, depth + 1));
    tree->addRight(constructKDTree(right, v2, depth + 1));
    return tree;
}

void Scene::prepare() {
    const double inf = 1e10;
  //  kdTree = constructKDTree(AABB(Vector3{-inf, -inf, inf}, Vector3{inf, inf, inf}), objects, 0);
}

