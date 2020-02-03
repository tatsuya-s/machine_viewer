cmake_minimum_required(VERSION 2.8)
project(GLCube)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-deprecated-declarations")

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

if(NOT ${OPENGL_FOUND})
  message("OPENGL not found")
endif()

include_directories(${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIRS})

add_executable(cubic cubic.c) 
target_link_libraries(cubic ${OPENGL_LIBRARY} ${GLUT_LIBRARY})