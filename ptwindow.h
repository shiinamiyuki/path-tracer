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

class PathTracerWindow : public Window{
    Scene scene;
    int spp;
public:
    PathTracerWindow(int w, int h,int _spp):scene(w,h),Window(w,h,"Path Tracer"){
        resize();
        spp = _spp;
    }
    void setCamPos(Vector3 t){
        scene.setCamPos(t);
    }
    void addObject(RenderObject * obj){
        scene.addObject(obj);
    }
    void show()override{
        scene.findALightSource();
        while (!glfwWindowShouldClose(window)) {
            update();
            glClear(GL_COLOR_BUFFER_BIT);
            paintGL();
            glfwSwapBuffers(window);

            glfwPollEvents();
        }
        glfwTerminate();
    }
    void paintGL(){
        /*glMatrixMode(GL_MODELVIEW);
        glBegin(GL_QUADS);
        glTexCoord2i(0, 0); glVertex2i(0,   0);
        glTexCoord2i(0, 1); glVertex2i(0,   height);
        glTexCoord2i(1, 1); glVertex2i(width, height);
        glTexCoord2i(1, 0); glVertex2i(width, 0);
        glEnd();*/
        glDrawPixels(width,height,GL_RGBA,GL_UNSIGNED_BYTE,scene.pixelBuffer.data());
    }
    void update(){
        scene.renderSamples(spp);
    }
};


#endif //PATH_TRACER_PTWINDOW_H
