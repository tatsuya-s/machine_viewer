#ifndef MACHINE_VIEWER_HPP
#define MACHINE_VIEWER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif
#include <Eigen/Core>
#include <Eigen/LU>
#include "glm.h"

void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void keyboard(unsigned char key, int x, int y);
void display(void);
void autoSpin(void);
void drawAxis(float scale);
void drawModel(const std::unique_ptr<GLMmodel>& model);
void getDragPos(double& px, double& py, double& pz, const int x, const int y, const Eigen::Vector4i& viewport);
void getMatrix();

void initCubic(int N);
void drawCubic(int N);
void drawCube(int N, int i);
void drawRawCube(int N);
void putStickers(int i, int N);

#endif // MACHINE_VIEWER_HPP