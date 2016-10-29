cmake_minimum_required(VERSION 2.8)

set(EXECUTABLE_OUTPUT_PATH ..)

include_directories(${FREETYPE_INCLUDE_DIRS})
link_directories(${FREETYPE_LIBRARY_DIRS})

add_library(rt
  Scene.cpp
  Vector.cpp
  Matrix.cpp
  Color.cpp
  Light.cpp
  Geometry.cpp
  Camera.cpp
  Sphere.cpp
)

add_executable(raytracer main.cpp)
target_link_libraries(raytracer
  ${FREETYPE_LIBRARIES}
  pngwriter
  rt
)

add_executable(rt_test test.cpp)
target_link_libraries(rt_test
  ${FREETYPE_LIBRARIES}
  pngwriter
  rt
)
