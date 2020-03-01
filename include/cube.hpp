#ifndef MACHINE_VIEWER_HPP
#define MACHINE_VIEWER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <cmath>
#include <bitset>
#include <random>
#include <map>
#include <utility>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif
#include <Eigen/Geometry>
#include <Eigen/LU>
#include "glm.h"

struct Action {
    GLubyte mask; // 8-bit mask: selected slices and/or models
    GLbyte  axis; // 0, +-1(x), +-2(y), +-3(z): rotation axis

    Action() : mask(0), axis(0) {}
    void clear() { mask = 0; axis = 0; }
};

void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void keyboard(unsigned char key, int x, int y);
void display();
void onTimer(int value);
void drawAxis(float scale);
void drawModel(const std::unique_ptr<GLMmodel>& model);
void getDragPos(double& px, double& py, double& pz, const int x, const int y, const Eigen::Vector4i& viewport);
void getMatrix();
void initCubic(std::string& move);
void drawCubic();
void drawCube(int i);
void drawRawCube();
void putStickers(int i);
void rotSlices();
void rotRandom();
bool makeAction(unsigned char key);
void makeMotionsFromSolution(std::string& solution, std::string& init_motion, std::string& solve_motion);

#endif // MACHINE_VIEWER_HPP