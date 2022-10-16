cmake_minimum_required(VERSION 3.10)

project(cube)

set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(cube cube.cpp ../glad.c)

target_link_libraries(cube glfw OpenGL::GL)
