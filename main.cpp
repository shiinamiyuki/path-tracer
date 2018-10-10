
#include "ptwindow.h"

#include "voxelmanager.h"

void CornellBox() {
    PathTracerWindow Window(1000, 1000, 5000);
    Window.addObject(new Sphere({-1e5, 0, 0}, Material(DIFF, {1, 0, 0}), 1e5 - 20)); //left
    Window.addObject(new Sphere({1e5, 0, 0}, Material(DIFF, {0, 1, 0}), 1e5 - 20)); //right
    Window.addObject(new Sphere({0, -1e5, 0}, Material(DIFF, {0.5, 0.5, 0.5}), 1e5 - 10)); //bottom
    Window.addObject(new Sphere({-3, -0.3, 10}, Material(SPEC, {1, 1, 1}, {}), 3));
    Window.addObject(new Sphere({1, -0.5, 2}, Material(SPEC, {1, 0, 0}, {0, 0, 0}), 0.4));
    Window.addObject(new Sphere({0, 0, 1e5},
                                Material(DIFF, {0.2, 0.2, 0.2}),
                                1e5 - 20));
    Window.addObject(new Sphere({0, 10000, 0},
                                Material(EMIT, {0, 0, 0}, {3, 3, 3}),
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
    PathTracerWindow window(1000, 1000, 4900);
    window.setCamPos({0, 1.5, -3});
    window.addObject(new Sphere({0, 0, 1e5},
                                Material(DIFF, {0.2, 0.2, 0.2}),
                                1e5 - 20));
    window.addObject(new Sphere({-10000, 0, 0},
                                Material(EMIT, {0, 0, 0},
                                         Vector3{0x66 / 255.0, (0xCC) / 255.0, (0xFF) / 255.0} * 2),
                                10000 - 200));
    window.addObject(new Sphere({0, -1e5, 0}, Material(DIFF, {0.5, 0.5, 0.5}), 1e5)); //bottom
    window.addObject(new Sphere({-1, 3, 3}, Material(SPEC, {1, 1, 1}), 3));
    window.addObject(new Sphere({0.6, 0.5, 0}, Material(DIFF, {1, 0, 0}), 0.5));
    window.show();
}

void cube() {
    PathTracerWindow window(1000, 1000, 5000);
    window.setCamPos({0, 1.5, -3});
    window.addObject(new Sphere({0, 0, 1e5},
                                Material(DIFF, {0.2, 0.2, 0.2}),
                                1e5 - 20));
    window.addObject(new Sphere({-10000, 0, 0},
                                Material(EMIT, {0, 0, 0}, {1, 1, 1}),
                                10000 - 200));
    window.addObject(new Sphere({0, -1e5, 0}, Material(DIFF, {0.5, 0.5, 0.5}), 1e5)); //bottom
    window.addObject(new Cube({-4, 0, 6}, Material(DIFF, {1, 1, 1}), 6));
    window.addObject(new Cube({0.6, 1, 0}, Material(DIFF, {.5, 0, 0}), 1));
    window.show();
}

void VoxelTest() {
    PathTracerWindow window(1000, 1000, 100);
    window.setCamPos({0, 1.5, -3});
    window.addObject(new Sphere({-1, 3, 3}, Material(DIFF, {1, 1, 1}), 3));
    window.addObject(new Sphere({0, 0, 1e5},
                                Material(DIFF, {0.2, 0.2, 0.2}),
                                1e5 - 20));
    window.addObject(new Sphere({-10000, 0, 0},
                                Material(DIFF, Vector3{1, 1, 1}),
                                10000 - 200));
    window.addObject(new Sphere({0, -1e5, 0}, Material(DIFF, {0.5, 0.5, 0.5}), 1e5)); //bottom*/
/*
    for(int x = -4;x<=4;x++){
        for(int z = -4;z<=4;z++){
            if(x*x+z*z <= 2)
                window.addObject(new Cube(Vector3(x,3 - (x*x+z*z) / 10,z)*0.2,Material(EMIT,{},Vector3(x/4 + 1,z/4 +1,0.5)*3 ), 0.2));
            else
                window.addObject(new Cube(Vector3(x,3 - (x*x+z*z) / 10,z)*0.2,Material(DIFF,Vector3(x/4 + 1,z/4 +1,0.5)), 0.2));

        }
    }*/
    auto voxelManager = new VoxelManager(Vector3{0, 0, -1}, 0.2, 12);
    for (int x = -4; x <= 4; x++) {
        for (int z = -4; z <= 4; z++) {
            if (x * x + z * z <= 2)
                voxelManager->addVoxel(x + 4, 5 - x * x + z * z, z + 4,
                                       Material(EMIT, {}, Vector3(x / 4.0 + 1, z / 4.0 + 1, 0.5) * 3));
            else
                voxelManager->addVoxel(x + 4, 5 - x * x + z * z, z + 4,
                                       Material(DIFF, Vector3(x / 4.0 + 1, z / 4.0 + 1, 0.5)));

        }
    }
    voxelManager->prepare();
    voxelManager->addLightSourceToScene(window.getScene());
    window.addObject(voxelManager);
    window.show();

}
void CubeSphere(){
    PathTracerWindow window(1000, 1000, 5000);
    window.setCamPos({0, 10, -100});
    window.addObject(new Sphere({0, 0, 1e5},
                                Material(DIFF, {0.2, 0.2, 0.2}),
                                1e5 - 20));
    window.addObject(new Sphere({-10000, 0, 0},
                                Material(EMIT, {0, 0, 0}, Vector3{1, 1, 1}),
                                10000 - 200));
    window.addObject(new Sphere({0, -1e5, 0}, Material(DIFF, {0.5, 0.5, 0.5}), 1e5)); //bottom*/
    auto voxelManager = new VoxelManager(Vector3{-5,0,-90},0.2,100);
    for(int i = 0;i<100;i++){
        for(int j = 0;j<100;j++){
            for(int k = 0;k<100;k++){
                double x = (i/100.0) * 2 - 1;
                double y = (j/100.0)*2 - 1;
                double z = (k/100.0)*2-1;
                x *= 50;
                y *= 50;
                z *= 50;
                if(sqrt(x*x+y*y+z*z)<=50){
                    voxelManager->addVoxel(
                            i,j,k,Material(DIFF,Vector3(i,j,k)*0.01)
                            );
                }
            }
        }
    }
    voxelManager->prepare();
    window.addObject(voxelManager);
    window.show();
}
int main() {
    glfwInit();
    VoxelTest();
}
