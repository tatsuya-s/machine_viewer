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

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include <Eigen/Geometry>
#include <Eigen/LU>

#include "trackball.h"
#include "glm.h"

struct Action {
    GLubyte mask; // 8-bit mask: selected slices and/or models
    GLbyte  axis; // 0, +-1(x), +-2(y), +-3(z): rotation axis

    Action() : mask(0), axis(0) {}
    void clear() { mask = 0; axis = 0; }
};

class MachineViewer {
public:
    MachineViewer();
    void initViewer(std::string& solution);
    bool isActive();
    void drawCube();
    void drawMachine();
    void swapBuffers();
    void updateCamera();
    void reshapeCallback();
    void keyboardCallback(int key, int action);
    void clickCallback(int button, int action);
    void motionCallback(double mouse_x, double mouse_y);
    void initTrackball();

    void drawAxis(float scale);
    void drawModel(const std::unique_ptr<GLMmodel>& model);
    void initCubic(std::string& move);
    void drawCubic();
    void drawCube(int i);
    void drawRawCube();
    void putStickers(int i);
    void rotSlices();
    void rotRandom();
    bool makeAction(int key);
    void makeMotionsFromSolution(std::string& solution, std::string& init_motion, std::string& solve_motion);
    void makeTrans();

private:
    static constexpr int N = 4;

    GLFWwindow* raw_window;
    int width, height;
    double prev_mouse_x, prev_mouse_y;
    bool mouse_left_pressed;
    bool mouse_middle_pressed;
    bool mouse_right_pressed;
    bool shift_pressed;
    std::array<float, 4> curr_quat, prev_quat;
    std::array<float, 3> eye, lookat, up;
    std::vector<std::unique_ptr<GLMmodel>> models;
    std::array<Eigen::Matrix4d, N*N*N> cubic, prev_cubic;
    std::array<uint8_t, N*N*N> slice;
    Action tmp_act, det_act;
    double dangle;
    double slice_angle;
    int scramble;
    bool rotating;
    bool skip_anim;
    bool auto_rotate;
    std::string::iterator solve_motion_it;
    std::string solve_motion;

    static constexpr std::array<float, 4> light_ambient  = { 0.0, 0.0, 0.0, 1.0 };
    static constexpr std::array<float, 4> light_diffuse  = { 1.0, 1.0, 1.0, 1.0 };
    static constexpr std::array<float, 4> light_specular = { 1.0, 1.0, 1.0, 1.0 };
    static constexpr std::array<float, 4> light_position = { 1.0, 1.0, 1.0, 0.0 };
    static constexpr std::array<float, 4> mat_ambient    = { 0.7, 0.7, 0.7, 1.0 };
    static constexpr std::array<float, 4> mat_diffuse    = { 0.8, 0.8, 0.8, 1.0 };
    static constexpr std::array<float, 4> mat_specular   = { 1.0, 1.0, 1.0, 1.0 };
    static constexpr std::array<float, 1> high_shininess = { 100.0 };

    inline static const std::array<Eigen::Vector3d, 7> axes = {
        Eigen::Vector3d( 0.0, 0.0,-1.0), 
        Eigen::Vector3d( 0.0,-1.0, 0.0), 
        Eigen::Vector3d(-1.0, 0.0, 0.0), 
        Eigen::Vector3d( 0.0, 0.0, 0.0),
        Eigen::Vector3d( 1.0, 0.0, 0.0), 
        Eigen::Vector3d( 0.0, 1.0, 0.0), 
        Eigen::Vector3d( 0.0, 0.0, 1.0)
    };

    inline static const std::array<Eigen::Matrix4d, 7> trans = {
        (Eigen::Matrix4d() << 0,-1, 0, 0,  1, 0, 0, 0,  0, 0, 1, 0,  0, 0, 0, 1).finished(),   /* -z */
        (Eigen::Matrix4d() << 0, 0, 1, 0,  0, 1, 0, 0, -1, 0, 0, 0,  0, 0, 0, 1).finished(),   /* -y */
        (Eigen::Matrix4d() << 1, 0, 0, 0,  0, 0,-1, 0,  0, 1, 0, 0,  0, 0, 0, 1).finished(),   /* -x */
        (Eigen::Matrix4d() << 1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  0, 0, 0, 1).finished(),   /*  0 */
        (Eigen::Matrix4d() << 1, 0, 0, 0,  0, 0, 1, 0,  0,-1, 0, 0,  0, 0, 0, 1).finished(),   /* +x */
        (Eigen::Matrix4d() << 0, 0,-1, 0,  0, 1, 0, 0,  1, 0, 0, 0,  0, 0, 0, 1).finished(),   /* +y */
        (Eigen::Matrix4d() << 0, 1, 0, 0, -1, 0, 0, 0,  0, 0, 1, 0,  0, 0, 0, 1).finished(),   /* +z */
    };

    inline static const std::map<std::string, std::string> dict = {
        {"R'", "1j"}, 
        {"D'", "1k"}, 
        {"F'", "1l"}, 
        {"L'", "4u"}, 
        {"U'", "4i"}, 
        {"B'", "4o"}, 
        {"R2'", "1j1j"}, 
        {"D2'", "1k1k"}, 
        {"F2'", "1l1l"}, 
        {"L2'", "4u4u"}, 
        {"U2'", "4i4i"}, 
        {"B2'", "4o4o"}, 
        {"Rw'", "12j"}, 
        {"Dw'", "12k"}, 
        {"Fw'", "12l"}, 
        {"Lw'", "34u"}, 
        {"Uw'", "34i"}, 
        {"Bw'", "34o"}, 
        {"Rw2'", "12j12j"}, 
        {"Dw2'", "12k12k"}, 
        {"Fw2'", "12l12l"}, 
        {"Lw2'", "34u34u"}, 
        {"Uw2'", "34i34i"}, 
        {"Bw2'", "34o34o"}, 
        {"Rw2", "12u12u"}, 
        {"Dw2", "12i12i"}, 
        {"Fw2", "12o12o"}, 
        {"Lw2", "34j34j"}, 
        {"Uw2", "34k34k"}, 
        {"Bw2", "34l34l"}, 
        {"Rw", "12u"}, 
        {"Dw", "12i"}, 
        {"Fw", "12o"}, 
        {"Lw", "34j"}, 
        {"Uw", "34k"}, 
        {"Bw", "34l"}, 
        {"R2", "1u1u"}, 
        {"D2", "1i1i"}, 
        {"F2", "1o1o"}, 
        {"L2", "4j4j"}, 
        {"U2", "4k4k"}, 
        {"B2", "4l4l"}, 
        {"R", "1u"}, 
        {"D", "1i"}, 
        {"F", "1o"}, 
        {"L", "4j"}, 
        {"U", "4k"}, 
        {"B", "4l"}
    };

    inline static const std::map<std::string, std::string> inverse_dict = {
        {"R'", "1u"}, 
        {"D'", "1i"}, 
        {"F'", "1o"}, 
        {"L'", "4j"}, 
        {"U'", "4k"}, 
        {"B'", "4l"}, 
        {"R2'", "1u1u"}, 
        {"D2'", "1i1i"}, 
        {"F2'", "1o1o"}, 
        {"L2'", "4j4j"}, 
        {"U2'", "4k4k"}, 
        {"B2'", "4l4l"}, 
        {"Rw'", "12u"}, 
        {"Dw'", "12i"}, 
        {"Fw'", "12o"}, 
        {"Lw'", "34j"}, 
        {"Uw'", "34k"}, 
        {"Bw'", "34l"}, 
        {"Rw2'", "12u12u"}, 
        {"Dw2'", "12i12i"}, 
        {"Fw2'", "12o12o"}, 
        {"Lw2'", "34j34j"}, 
        {"Uw2'", "34k34k"}, 
        {"Bw2'", "34l34l"}, 
        {"Rw2", "12j12j"}, 
        {"Dw2", "12k12k"}, 
        {"Fw2", "12l12l"}, 
        {"Lw2", "34u34u"}, 
        {"Uw2", "34i34i"}, 
        {"Bw2", "34o34o"}, 
        {"Rw", "12j"}, 
        {"Dw", "12k"}, 
        {"Fw", "12l"}, 
        {"Lw", "34u"}, 
        {"Uw", "34i"}, 
        {"Bw", "34o"}, 
        {"R2", "1j1j"}, 
        {"D2", "1k1k"}, 
        {"F2", "1l1l"}, 
        {"L2", "4u4u"}, 
        {"U2", "4i4i"}, 
        {"B2", "4o4o"}, 
        {"R", "1j"}, 
        {"D", "1k"}, 
        {"F", "1l"}, 
        {"L", "4u"}, 
        {"U", "4i"}, 
        {"B", "4o"}
    };
};

#endif // MACHINE_VIEWER_HPP