cmake_minimum_required(VERSION 3.10)

project(static_system)

set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(static_system static_system.cpp ../glad.c)

target_link_libraries(static_system glfw OpenGL::GL)
