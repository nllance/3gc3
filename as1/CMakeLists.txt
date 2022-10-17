cmake_minimum_required(VERSION 3.10)

project(dynamic_system)

set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(dynamic_system dynamic_system.cpp ../glad.c)

target_link_libraries(dynamic_system glfw OpenGL::GL)
