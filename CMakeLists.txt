cmake_minimum_required(VERSION 3.12)
project(path_tracer)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp ")
add_executable(path_tracer main.cpp lib/lodepng/lodepng.cpp
        lib/rand48/rand48.c lib/tiny_obj_loader/tiny_obj_loader.cc
        format.cc posix.cc)