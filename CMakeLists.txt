cmake_minimum_required(VERSION 2.8)
project(GLCube)
set(CMAKE_C_FLAGS "-Wall -Wno-error=deprecated-declarations -Wno-deprecated-declarations")
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wno-error=deprecated-declarations -Wno-deprecated-declarations")

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories(
  glm 
  ${OPENGL_INCLUDE_DIR} 
  ${GLUT_INCLUDE_DIRS}
)

add_library(
  libglm STATIC
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
  libglm 
    ${OPENGL_LIBRARY} 
    ${GLUT_LIBRARY}
)

add_executable(
  cubic 
    cubic.c
) 
target_link_libraries(
  cubic 
    ${OPENGL_LIBRARY} 
    ${GLUT_LIBRARY}
)

add_executable(
  glutobj 
    glutobj.c) 
target_link_libraries(
  glutobj 
    libglm
    ${OPENGL_LIBRARY} 
    ${GLUT_LIBRARY}
)

add_executable(replace replace.cpp) 