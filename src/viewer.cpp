#include "viewer.hpp"

MachineViewer::MachineViewer() : 
    mouse_left_pressed(false), 
    mouse_middle_pressed(false), 
    mouse_right_pressed(false), 
    shift_pressed(false)
{
    if(!glfwInit()) {
        std::cerr << "glfwInit failed" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    raw_window = glfwCreateWindow(960, 640, "Window", nullptr, nullptr);

    if(!raw_window) {
        std::cerr << "glfwCreateWindow failed" << std::endl;
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(raw_window);
    glfwSetWindowUserPointer(raw_window, this);

    reshapeCallback();

    // https://stackoverflow.com/questions/7676971/pointing-to-a-function-that-is-a-class-member-glfw-setkeycallback
    auto reshape_func = [](GLFWwindow* w, int, int) {
        static_cast<MachineViewer*>(glfwGetWindowUserPointer(w))->reshapeCallback();
    };
    glfwSetWindowSizeCallback(raw_window, reshape_func);

    auto keyboard_func = [](GLFWwindow* w, int key, int, int action, int) {
        static_cast<MachineViewer*>(glfwGetWindowUserPointer(w))->keyboardCallback(key, action);
    };
    glfwSetKeyCallback(raw_window, keyboard_func);

    auto click_func = [](GLFWwindow* w, int button, int action, int) {
        static_cast<MachineViewer*>(glfwGetWindowUserPointer(w))->clickCallback(button, action);
    };
    glfwSetMouseButtonCallback(raw_window, click_func);

    auto motion_func = [](GLFWwindow* w, double mouse_x, double mouse_y) {
        static_cast<MachineViewer*>(glfwGetWindowUserPointer(w))->motionCallback(mouse_x, mouse_y);
    };
    glfwSetCursorPosCallback(raw_window, motion_func);
}

void MachineViewer::initViewer() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glCullFace(GL_FRONT);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, this->aLightColor);
    glLightfv(GL_LIGHT1, GL_SPECULAR, this->aLightColor);
    glClearColor(1, 1, 1, 1);
}

bool MachineViewer::isActive() {
    bool status = glfwWindowShouldClose(this->raw_window);
    if (status) {
        glfwTerminate();
    }
    return !status;
}

void MachineViewer::drawLight() {
    glLightfv(GL_LIGHT0, GL_POSITION, this->aLight0pos);
    glLightfv(GL_LIGHT1, GL_POSITION, this->aLight1pos);
}

void MachineViewer::drawCube() {
    glTranslated( -0.5, -0.5, -0.5 );
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, this->aCubeMaterial);
    glBegin(GL_QUADS);
    for (size_t i = 0; i < 6; ++i) {
        glNormal3dv(this->aCubeNormal[i]);
        for (size_t j = 0; j < 4; ++j) {
            glVertex3dv(this->aCubeVertex[ this->aCubeFace[i][j] ]);
        }
    }
    glEnd();
}

void MachineViewer::swapBuffers() {
    glfwSwapBuffers(this->raw_window);
}

void MachineViewer::updateCamera() {
    GLfloat mat[4][4];
    gluLookAt(eye[0], eye[1], eye[2], lookat[0], lookat[1], lookat[2], up[0], up[1], up[2]);
    build_rotmatrix(mat, curr_quat.data());
    glMultMatrixf(&mat[0][0]);
}

void MachineViewer::reshapeCallback() {
    glfwGetFramebufferSize(this->raw_window, &(this->width), &(this->height));
    glViewport(0, 0, this->width, this->height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)this->width / (float)this->height, 0.01f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void MachineViewer::keyboardCallback(int key, int action) {
    if (action == GLFW_PRESS) { // || action == GLFW_REPEAT)
        if ((key == GLFW_KEY_LEFT_SHIFT) | (key == GLFW_KEY_RIGHT_SHIFT)) {
            shift_pressed = true;
        }
        if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(raw_window, GL_TRUE);
    }
    else {
        shift_pressed = false;
    }
}

void MachineViewer::clickCallback(int button, int action) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            if (shift_pressed) {
                mouse_middle_pressed = true;
            }
            else {
                mouse_left_pressed = true;
                trackball(prev_quat.data(), 0.0, 0.0, 0.0, 0.0);
            }
        } else if (action == GLFW_RELEASE) {
            mouse_left_pressed = false;
            mouse_middle_pressed = false;
        }
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            mouse_right_pressed = true;
        } else if (action == GLFW_RELEASE) {
            mouse_right_pressed = false;
        }
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        if (action == GLFW_PRESS) {
            mouse_middle_pressed = true;
        } else if (action == GLFW_RELEASE) {
            mouse_middle_pressed = false;
        }
    }
}

void MachineViewer::motionCallback(double mouse_x, double mouse_y) {
    const float rot_scale = 2.0f;
    const float trans_scale = 4.0f;

    if (mouse_left_pressed) {
        trackball(prev_quat.data(), rot_scale * (2.0f * prev_mouse_x - width) / (float)width,
                                    rot_scale * (height - 2.0f * prev_mouse_y) / (float)height,
                                    rot_scale * (2.0f * mouse_x - width) / (float)width,
                                    rot_scale * (height - 2.0f * mouse_y) / (float)height);

        add_quats(prev_quat.data(), curr_quat.data(), curr_quat.data());
    } else if (mouse_middle_pressed) {
        eye[0] -= trans_scale * (mouse_x - prev_mouse_x) / (float)width;
        lookat[0] -= trans_scale * (mouse_x - prev_mouse_x) / (float)width;
        eye[1] += trans_scale * (mouse_y - prev_mouse_y) / (float)height;
        lookat[1] += trans_scale * (mouse_y - prev_mouse_y) / (float)height;
    } else if (mouse_right_pressed) {
        eye[2] += trans_scale * (mouse_y - prev_mouse_y) / (float)height;
        lookat[2] += trans_scale * (mouse_y - prev_mouse_y) / (float)height;
    }

    prev_mouse_x = mouse_x;
    prev_mouse_y = mouse_y;
}

void MachineViewer::initTrackball() {
    trackball(this->curr_quat.data(), 0, 0, 0, 0);

    this->eye = {0.0f, 0.0f, 3.0f};
    this->lookat = {0.0f, 0.0f, 0.0f};
    this->up = {0.0f, 1.0f, 0.0f};
}