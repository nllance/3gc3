cmake_minimum_required(VERSION 3.10)

project(as0)

set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(as0 assignment0.cpp ../glad.c)
target_link_libraries(as0 OpenGL::GL)
target_link_libraries(as0 glfw)
