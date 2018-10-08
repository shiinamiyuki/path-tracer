
#include "ptwindow.h"

class RenderObject;


void CornellBox() {
    PathTracerWindow Window(500, 500,5000);
    Window.addObject(new Sphere({-1e5, 0, 0}, Material(DIFF, {1, 0, 0}), 1e5 - 20)); //left
    Window.addObject(new Sphere({1e5, 0, 0}, Material(DIFF, {0, 1, 0}), 1e5 - 20)); //right
    Window.addObject(new Sphere({0, -1e5, 0}, Material(DIFF, {0.5, 0.5, 0.5}), 1e5 - 10)); //bottom
    Window.addObject(new Sphere({-3, -0.3, 10}, Material(SPEC, {1, 1, 1}, {}), 3));
    Window.addObject(new Sphere({1, -0.5, 2}, Material(SPEC, {1, 0, 0}, {0, 0, 0}), 0.4));
    Window.addObject(new Sphere({0, 0, 1e5},
                               Material(DIFF, {0.2, 0.2, 0.2}),
                               1e5 - 20));
    Window.addObject(new Sphere({0, 10000, 0},
                               Material(EMIT, {0, 0, 0}, {12, 12, 12}),
                               10000 - 200));
    Window.show();
}

void wiki() {
    Scene scene(500, 500);
    scene.setCamPos({0, 1.5, -3});
    scene.addObject(new Sphere({0, 0, 1e5},
                               Material(DIFF, {0.2, 0.2, 0.2}),
                               1e5 - 20));
    scene.addObject(new Sphere({-10000, 0, 0},
                               Material(EMIT, {0, 0, 0}, {1, 1, 1}),
                               10000 - 200));
    scene.addObject(new Sphere({0, -1e5, 0}, Material(DIFF, {0.5, 0.5, 0.5}), 1e5)); //bottom
    scene.addObject(new Sphere({-1, 3, 3}, Material(DIFF, {1, 1, 1}), 3));
    scene.addObject(new Sphere({0.6, 0.5, 0}, Material(DIFF, {1, 0, 0}), 0.5));
    scene.render(4900);
}


void wikiWindow() {
    PathTracerWindow window(1000, 1000,4900);
    window.setCamPos({0, 1.5, -3});
    window.addObject(new Sphere({0, 0, 1e5},
                               Material(DIFF, {0.2, 0.2, 0.2}),
                               1e5 - 20));
    window.addObject(new Sphere({-10000, 0, 0},
                               Material(EMIT, {0, 0, 0}, {1, 1, 1}),
                               10000 - 200));
    window.addObject(new Sphere({0, -1e5, 0}, Material(DIFF, {0.5, 0.5, 0.5}), 1e5)); //bottom
    window.addObject(new Sphere({-1, 3, 3}, Material(DIFF, {1, 1, 1}), 3));
    window.addObject(new Sphere({0.6, 0.5, 0}, Material(DIFF, {1, 0, 0}), 0.5));
    window.show();
}
void cube() {
    PathTracerWindow window(1000, 1000,5000);
    window.setCamPos({0, 1.5, -3});
    window.addObject(new Sphere({0, 0, 1e5},
                                Material(DIFF, {0.2, 0.2, 0.2}),
                                1e5 - 20));
    window.addObject(new Sphere({-10000, 0, 0},
                                Material(EMIT, {0, 0, 0}, {1, 1, 1}),
                                10000 - 200));
    window.addObject(new Sphere({0, -1e5, 0}, Material(DIFF, {0.5, 0.5, 0.5}), 1e5)); //bottom
    window.addObject(new Cube({-4, 0, 6}, Material(SPEC, {1, 1, 1}), 6));
    window.addObject(new Cube({0.6, 1, 0}, Material(DIFF, {1, 0, 0}), 1));
    window.show();
}
int main() {
    glfwInit();
    cube();
}
