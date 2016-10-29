cmake_minimum_required(VERSION 2.8)
project(RayTracer)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")

# get Freetype
find_package(Freetype REQUIRED)

# pngwriter
add_subdirectory(pngwriter)
include_directories(pngwriter/src)

# my code
add_subdirectory(src)
