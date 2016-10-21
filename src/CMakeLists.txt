cmake_minimum_required(VERSION 2.8)

add_executable(raytracer main.cpp)
set(EXECUTABLE_OUTPUT_PATH ..)

add_library(rt Scene.cpp Vector.cpp Camera.cpp Sphere.cpp)
target_link_libraries(raytracer rt)
