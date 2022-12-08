cmake_minimum_required(VERSION 3.10)

project(ray)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCES main.cpp headers/Camera.h headers/Material.h headers/Ray.h headers/Sphere.h headers/Vector3D.h headers/World.h)
add_executable(ray ${SOURCES})

target_include_directories(ray PRIVATE headers)
