cmake_minimum_required(VERSION 3.10)

project(spot_light)

set(OpenGL_GL_PREFERENCE GLVND)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

set(SOURCES spot_light.cpp ../headers/shader.h ../headers/mesh.h ../headers/model.h ../../tiny_obj_loader.h ../../glad.c ../../stb_image.h)
add_executable(spot_light ${SOURCES})

target_include_directories(spot_light PRIVATE ../headers)
target_link_libraries(spot_light glfw OpenGL::GL)
