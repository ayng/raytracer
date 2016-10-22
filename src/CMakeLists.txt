cmake_minimum_required(VERSION 2.8)

add_executable(raytracer main.cpp)
set(EXECUTABLE_OUTPUT_PATH ..)

include_directories(${FREETYPE_INCLUDE_DIRS})
link_directories(${FREETYPE_LIBRARY_DIRS})

add_library(rt
  Scene.cpp
  Vector.cpp
  Color.cpp
  Camera.cpp
  Sphere.cpp
)
target_link_libraries(raytracer
  ${FREETYPE_LIBRARIES}
  pngwriter
  rt
)
