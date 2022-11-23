cmake_minimum_required(VERSION 3.10)

project(point_light_bucket)

set(OpenGL_GL_PREFERENCE GLVND)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

set(SOURCES point_light.cpp ../headers/shader.h ../headers/mesh.h ../headers/model.h ../../tiny_obj_loader.h ../../glad.c ../../stb_image.h)
add_executable(point_light_bucket ${SOURCES})

target_include_directories(point_light_bucket PRIVATE ../headers)
target_link_libraries(point_light_bucket glfw OpenGL::GL)
