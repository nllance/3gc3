cmake_minimum_required(VERSION 3.10)

project(test)

set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(test assignment0.cpp glad.c)
target_link_libraries(test OpenGL::GL)
target_link_libraries(test glfw)
