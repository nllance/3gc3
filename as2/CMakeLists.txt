cmake_minimum_required(VERSION 3.10)

project(faces)

set(OpenGL_GL_PREFERENCE GLVND)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

set(SOURCES faces.cpp shader.h mesh.h model.h tiny_obj_loader.h ../glad.c)
add_executable(faces ${SOURCES})

target_link_libraries(faces glfw OpenGL::GL)
