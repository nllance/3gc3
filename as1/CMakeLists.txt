cmake_minimum_required(VERSION 3.10)

project(rotate_system)

set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(rotate_system rotate_system.cpp ../glad.c)

target_link_libraries(rotate_system glfw OpenGL::GL)
