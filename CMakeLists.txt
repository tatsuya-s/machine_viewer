cmake_minimum_required(VERSION 2.8)
project(machine-viewer)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS "-Wall -Wno-error=deprecated-declarations -Wno-deprecated-declarations")
set(CMAKE_CXX_FLAGS "-Wall -Wno-error=deprecated-declarations -Wno-deprecated-declarations -Wno-register")

find_package(OpenGL REQUIRED)
find_package(GLFW REQUIRED)
find_package(GLUT REQUIRED)
find_package(Eigen3 REQUIRED)

include_directories(
  include
  glm
  ${OPENGL_INCLUDE_DIR}
  ${GLFW_INCLUDE_DIR}
  ${GLUT_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIR}
)

add_library(
  glm STATIC
    glm/glm.c 
    glm/glm_util.c 
    glm/glmimg.c 
    glm/glmimg_jpg.c 
    glm/glmimg_png.c 
    glm/glmimg_sdl.c 
    glm/glmimg_sim.c 
    glm/glmimg_devil.c
)
target_link_libraries(
  glm 
    ${OPENGL_LIBRARY} 
    ${GLUT_LIBRARY}
)

add_library(
  trackball STATIC
    src/trackball.cpp
)
target_link_libraries(
  trackball 
    m
)

add_executable(
  viewer 
    src/main.cpp
    src/viewer.cpp)
target_link_libraries(
  viewer 
    glm
    trackball
    ${OPENGL_LIBRARY}
    ${GLFW_LIBRARIES}
)