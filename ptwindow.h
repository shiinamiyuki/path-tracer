//
// Created by xiaoc on 2018/10/8.
//

#ifndef PATH_TRACER_PTWINDOW_H
#define PATH_TRACER_PTWINDOW_H

#include <iostream>
#include <cmath>
#include <vector>
#include <random>
#include <chrono>
#include "lib/lodepng/lodepng.h"
#include "fmt/format.h"
#include "lib/rand48/erand48.h"
#include "renderobject.h"
#include "scene.h"
#include "ray.h"
#include "material.h"
#include "sphere.h"
#include "cube.h"
#include "util.h"
#include "window.h"

class PathTracerWindow : public Window {
protected:
    Scene scene;
    int spp;
    std::vector<unsigned char> pixels;
    volatile bool alive;
    std::string logOutput;
public:
    PathTracerWindow(int w, int h, int _spp) : scene(w, h), Window(w, h, "Path Tracer") {
        resize();
        spp = _spp;
        alive = true;
    }

    void setCamPos(Vector3 t) {
        scene.setCamPos(t);
    }

    void addObject(RenderObject *obj) {
        scene.addObject(obj);
    }

    void log(const std::string &s) {
        logOutput.append(s);
    }

    void save();

    void render() {
        std::thread thread([&]() {
            while (alive) {
                scene.renderSamples(spp);
                pixels = scene.pixelBuffer;
            }
        });
        thread.detach();
    }

    static double sec() {
        return (double) clock() / CLOCKS_PER_SEC;
    }

    void show() override {
        auto start = sec();
        scene.findALightSource();
        scene.prepare();
        render();
        while (!glfwWindowShouldClose(window)) {
            update();
            glClear(GL_COLOR_BUFFER_BIT);
            paintGL();
            glfwSwapBuffers(window);
            glfwPollEvents();
            auto t = sec() - start;
            auto total = 1/scene.sampleCount * spp * t;
            glfwSetWindowTitle(
                    window,
                    fmt::format("Path Tracer ({0}/{1} spp) Elapsed: {2}s, Remaining: {3}s",
                                scene.sampleCount,
                                spp,
                                t,
                                total - t).c_str()


            );
            Sleep(1000 / 30);
        }
        glfwTerminate();
        alive = false;
        Sleep(500);
        save();
    }

    void paintGL() {
        glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    }

    void update() {
    }
    Scene& getScene(){return scene;}
};

class DebugWindow {
    Scene scene;
    int spp;
public:
    DebugWindow(int w, int h, int _spp) : scene(w, h) {
        spp = _spp;
    }

    void setCamPos(Vector3 t) {
        scene.setCamPos(t);
    }

    void addObject(RenderObject *obj) {
        scene.addObject(obj);
    }

    void show() {
        scene.findALightSource();
        scene.prepare();
        while (true) {
            scene.renderSamples(spp, 4);
            Sleep(100);
        }
    }

    void trace(int x, int y) {
        scene.findALightSource();
        scene.prepare();
        unsigned short Xi[] = {0, 0, x * x * x};
        scene.trace(x, y, sqrt(spp), Xi);
    }
};


#endif //PATH_TRACER_PTWINDOW_H
