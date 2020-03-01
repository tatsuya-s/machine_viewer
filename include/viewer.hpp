#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <array>
#include "trackball.h"

class MachineViewer {
public:
    MachineViewer();
    void initViewer();
    bool isActive();
    void drawLight();
    void drawCube();
    void swapBuffers();
    void updateCamera();
    void reshapeCallback();
    void keyboardCallback(int key, int action);
    void clickCallback(int button, int action);
    void motionCallback(double mouse_x, double mouse_y);
    void initTrackball();

private:
    void clickCallback();

    GLFWwindow* raw_window;
    int width, height;
    double prev_mouse_x, prev_mouse_y;
    bool mouse_left_pressed;
    bool mouse_middle_pressed;
    bool mouse_right_pressed;
    bool shift_pressed;
    std::array<float, 4> curr_quat, prev_quat;
    std::array<float, 3> eye, lookat, up;

    inline static const GLdouble aCubeVertex[][3] = {
        { 0.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 }, 
        { 1.0, 1.0, 0.0 }, 
        { 0.0, 1.0, 0.0 }, 
        { 0.0, 0.0, 1.0 }, 
        { 1.0, 0.0, 1.0 }, 
        { 1.0, 1.0, 1.0 }, 
        { 0.0, 1.0, 1.0 } 
    };
    inline static const int aCubeFace[][4] = {
        { 0, 1, 2, 3 },
        { 1, 5, 6, 2 },
        { 5, 4, 7, 6 },
        { 4, 0, 3, 7 },
        { 4, 5, 1, 0 },
        { 3, 2, 6, 7 }
    };
    inline static const GLdouble aCubeNormal[][3] = {
        { 0.0, 0.0,-1.0 },
        { 1.0, 0.0, 0.0 },
        { 0.0, 0.0, 1.0 },
        {-1.0, 0.0, 0.0 },
        { 0.0,-1.0, 0.0 },
        { 0.0, 1.0, 0.0 }
    };

    inline static const GLfloat aCubeMaterial[] = { 0.8, 0.2, 0.2, 1.0 };
    inline static const GLfloat aLightColor[] = { 0.2, 0.2, 0.8, 1.0 };
    inline static const GLfloat aLight0pos[] = { 0.0, 3.0, 5.0, 1.0 };
    inline static const GLfloat aLight1pos[] = { 5.0, 3.0, 0.0, 1.0 };
};