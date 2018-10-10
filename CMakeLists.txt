cmake_minimum_required(VERSION 3.12)
project(path_tracer)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
link_libraries(glfw3 gdi32 opengl32)
add_executable(path_tracer main.cpp lib/lodepng/lodepng.cpp
        lib/rand48/rand48.c lib/tiny_obj_loader/tiny_obj_loader.cc
        format.cc posix.cc scene.h renderobject.h material.h vector3.h ray.h util.h sphere.h cube.h ptwindow.h scene.cc ptwindow.cc kdtree.cc kdtree.h aabb.h hit.h voxelmanager.h voxelmanager.cc)