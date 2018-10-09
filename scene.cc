//
// Created by xiaoc on 2018/10/9.
//

#include "scene.h"
void Scene::renderSamples(int spp, int samples) {
    const double xmax = 1, ymax = 1;
    const double sppSqrt = (double) sqrt(spp);
    if(done)
        return;
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
static double frand(){
    static std::random_device seeder;
    static std::mt19937 engine(seeder());
    static std::uniform_real_distribution<double> dist(1, 6);

    return dist(engine);
}
static Vector3 randomVector(const Vector3& norm,unsigned short* Xi){
    double r1 = 2 * M_PI * erand48(Xi), r2 = erand48(Xi), r2s = sqrt(r2);
    auto w = norm;
    auto u = Vector3::cross((abs(w.x) > 0.1) ? Vector3{0, 1, 0} : Vector3{1, 0, 0}, w).norm();
    auto v = Vector3::cross(w, u);
    auto r = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).norm();
    return r;
}
static Vector3 randomVectorInCone(const Vector3& norm, double rad,unsigned short *Xi){
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
    Vector3 color = {0,0,0};
    Vector3 r;
    if (object->material.type == SPEC) {
        double roughness = 0;
        r = ray.direction - norm * 2 * (ray.direction * norm);
        r += Vector3(erand48(Xi) - 0.5, erand48(Xi) - 0.5, erand48(Xi) - 0.5) * roughness;
        r.normalize();
    } else if (object->material.type == DIFF) {
        if (norm * ray.direction > 0)
            norm *= -1;
       /* double r1 = 2 * M_PI * erand48(Xi), r2 = erand48(Xi), r2s = sqrt(r2);
        auto w = norm;
        auto u = Vector3::cross((abs(w.x) > 0.1) ? Vector3{0, 1, 0} : Vector3{1, 0, 0}, w).norm();
        auto v = Vector3::cross(w, u);
        r = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).norm();*/
       r = randomVector(norm ,Xi);
        auto light = findALightSource();
        auto prob = 0.4;
        auto rad = 0.04;
        if(light && erand48(Xi) <prob){
            auto p = ray.getIntersectionPoint(hit.distance);
            auto n = randomVectorInCone((light->center - p).norm(),rad,Xi);
            auto r2 = Ray(p, n);
           auto tr = trace(r2,maxDepth - 1,Xi);
           color += tr * (n * norm);
        }
        //r = randomVectorInHemisphere(norm,Xi);
    } else
        return emit;
    auto newRay = Ray(ray.getIntersectionPoint(hit.distance), r);
    const double p = 1 / (2 * M_PI);
    const double cos_theta = newRay.direction * norm;
    auto BRDF = material.reflectance * (1 / M_PI);
    auto incoming = trace(newRay, depth + 1, Xi);
    auto refl = (BRDF.mult(incoming) * (cos_theta / p));
    color += emit + refl;
    return color;
}

RenderObject *Scene::findALightSource() {
    static bool first = true;
    static std::vector<RenderObject*>vec;
    static int cnt = 0;
    if(first){
        for(auto i:objects){
            if(i->material.type == EMIT){
                vec.push_back(i);
            }
        }
        first =false;
    }
    if(vec.empty())
        return nullptr;
    else{
        if(cnt >= vec.size())
            cnt =0 ;
        return vec[cnt++];
    }
}
