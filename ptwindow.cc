//
// Created by xiaoc on 2018/10/9.
//

#include "ptwindow.h"

void PathTracerWindow::save() {
    scene.save();
    lodepng::encode("out.png", scene.pixelBuffer, width, height);
}
