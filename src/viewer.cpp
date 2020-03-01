#include "viewer.hpp"

MachineViewer::MachineViewer() : 
    mouse_left_pressed(false), 
    mouse_middle_pressed(false), 
    mouse_right_pressed(false), 
    shift_pressed(false),
    dangle(90.0 / 10),
    slice_angle(0),
    scramble(0),
    rotating(false),
    skip_anim(false), 
    auto_rotate(false)
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

void MachineViewer::initViewer(std::string& solution) {
    initTrackball();

    std::vector<std::string> files;
    files.push_back("../obj/main.obj");
    files.push_back("../obj/bc_l_body.obj");
    files.push_back("../obj/bc_l_tl.obj");
    files.push_back("../obj/bc_l_tr.obj");
    files.push_back("../obj/bc_l_bl.obj");
    //files.push_back("../obj/bc_l_br.obj");
    files.push_back("../obj/bc_r_body.obj");
    files.push_back("../obj/bc_r_tl.obj");
    files.push_back("../obj/bc_r_tr.obj");
    files.push_back("../obj/bc_r_bl.obj");
    files.push_back("../obj/bc_r_br.obj");
    files.push_back("../obj/ma_t_body.obj");
    files.push_back("../obj/ma_t_masu.obj");
    files.push_back("../obj/ma_b_body.obj");
    files.push_back("../obj/ma_b_masu.obj");
    files.push_back("../obj/camera.obj");

    freopen("/dev/null", "w", stderr);
    for (const auto& file : files) {
        GLMmodel* model = glmReadOBJ(file.c_str());
        models.push_back(std::unique_ptr<GLMmodel>(model));
    }
    
    std::string init_motion;
    makeMotionsFromSolution(solution, init_motion, solve_motion);
    solve_motion_it = solve_motion.begin();

    std::cout << solution << std::endl;
    std::cout << init_motion << std::endl;
    std::cout << solve_motion << std::endl;

    initCubic(init_motion);

    glClearColor(0.05f, 0.1f, 0.15f, 1.0f);

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient.data());
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse.data());
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular.data());
    glLightfv(GL_LIGHT0, GL_POSITION, light_position.data());

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient.data());
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse.data());
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular.data());
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess.data());

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);
}

bool MachineViewer::isActive() {
    bool status = glfwWindowShouldClose(this->raw_window);
    if (status) {
        glfwTerminate();
    }
    return !status;
}

void MachineViewer::drawCube() {
    const GLdouble aCubeVertex[][3] = {
        { 0.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 }, 
        { 1.0, 1.0, 0.0 }, 
        { 0.0, 1.0, 0.0 }, 
        { 0.0, 0.0, 1.0 }, 
        { 1.0, 0.0, 1.0 }, 
        { 1.0, 1.0, 1.0 }, 
        { 0.0, 1.0, 1.0 } 
    };
    const int aCubeFace[][4] = {
        { 0, 1, 2, 3 },
        { 1, 5, 6, 2 },
        { 5, 4, 7, 6 },
        { 4, 0, 3, 7 },
        { 4, 5, 1, 0 },
        { 3, 2, 6, 7 }
    };
    const GLdouble aCubeNormal[][3] = {
        { 0.0, 0.0,-1.0 },
        { 1.0, 0.0, 0.0 },
        { 0.0, 0.0, 1.0 },
        {-1.0, 0.0, 0.0 },
        { 0.0,-1.0, 0.0 },
        { 0.0, 1.0, 0.0 }
    };
    const GLfloat aCubeMaterial[] = { 0.8, 0.2, 0.2, 1.0 };

    glTranslated( -0.5, -0.5, -0.5 );
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, aCubeMaterial);
    glBegin(GL_QUADS);
    for (size_t i = 0; i < 6; ++i) {
        glNormal3dv(aCubeNormal[i]);
        for (size_t j = 0; j < 4; ++j) {
            glVertex3dv(aCubeVertex[ aCubeFace[i][j] ]);
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
        std::cout << static_cast<unsigned char>(key) << std::endl;
        if ((key == GLFW_KEY_LEFT_SHIFT) | (key == GLFW_KEY_RIGHT_SHIFT)) {
            shift_pressed = true;
        }
        else if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(raw_window, GL_TRUE);
        }
        else if (key == GLFW_KEY_S) {
            scramble = 30;
        }
        else if (key == GLFW_KEY_A) {
            auto_rotate = !auto_rotate;
        }
        else if (makeAction(key)) {
            rotSlices();
        }
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

    this->eye = {0.0f, 0.0f, 0.5f};
    this->lookat = {0.0f, 0.0f, 0.0f};
    this->up = {0.0f, 1.0f, 0.0f};
}

void MachineViewer::drawMachine() {
    //glTranslatef(0, 0, -0.5);
    glRotatef( 20, 1, 0, 0);
    glRotatef(-20, 0, 1, 0);
    glRotatef( -2, 0, 0, 1);

    drawAxis(0.055); // 55mm
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    for (const auto& model : models) {
        drawModel(model);
    }

    drawCubic();

    glPopMatrix();
}

void MachineViewer::drawAxis(float scale) {
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glScalef(scale, scale, scale);
    glLineWidth(5.0);

    glBegin(GL_LINES);

    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(.8f, 0.05f, 0.0);
    glVertex3f(1.0, 0.25f, 0.0); //  Letter X
    glVertex3f(0.8f, .25f, 0.0);
    glVertex3f(1.0, 0.05f, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, 0.0, 0.0); // X axis

    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 1.0, 0.0); // Y axis

    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 1.0); // Z axis
    glEnd();
    glEnable(GL_LIGHTING);
    glColor3f(1.0, 1.0, 1.0);
    glPopMatrix();
}

void MachineViewer::drawModel(const std::unique_ptr<GLMmodel>& model) {
    glPushMatrix();
    glScalef(0.001, 0.001, 0.001);
    glmDraw(model.get(), GLM_FLAT | GLM_COLOR);
    glPopMatrix();
}

void MachineViewer::initCubic(std::string& init) {
    for (int i = 0; i < N*N*N; ++i) {
        cubic[i] = Eigen::MatrixXd::Identity(4, 4);
        cubic[i](0,3) = (i%N)-0.5*(N-1);
        cubic[i](1,3) = (i%(N*N))/N-0.5*(N-1);
        cubic[i](2,3) = (i/(N*N)) - 0.5*(N-1);

        for (auto& c : init) {
            if (!makeAction(c)) {
                continue;
            }
            
            int axis_row = std::abs(tmp_act.axis) - 1;
            slice[i] = static_cast<uint8_t>(cubic[i](axis_row, 3) + 0.5 * (N - 1));

            if (tmp_act.mask & (1 << slice[i])) {
                cubic[i] = trans[tmp_act.axis + 3] * cubic[i];
            }

            tmp_act.clear();
        }
    }
}

void MachineViewer::drawCubic() {
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glScaled(0.0156, 0.0156, 0.0156); // 4 x 0.015625 = 0.0625

    const Eigen::Affine3d affine = Eigen::Affine3d(Eigen::AngleAxisd(-dangle * M_PI / 180, axes[det_act.axis + 3]));

    for (int i = 0; i < N*N*N; ++i) {
        if (det_act.mask & (1 << slice[i])) {
            if ((skip_anim) | (slice_angle > 90)) {
                cubic[i] = trans[det_act.axis + 3] * prev_cubic[i];
            }
            else if (slice_angle > 0) {
                cubic[i] = affine.matrix() * cubic[i];
            }
        }
        drawCube(i);
    }

    if (skip_anim | (slice_angle > 90)) {
        det_act.clear();
        prev_cubic = cubic;
        slice_angle = 0;
        rotating = false;
    }

    slice_angle = slice_angle > 0 ? slice_angle + dangle : 0;

    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void MachineViewer::drawCube(int i) {
    int N1 = N-1;
    int j = (i%N), k = (i%(N*N))/N, l = (i/(N*N));
    if (j==0 || j-N1==0 || k==0 || k-N1==0 || l==0 || l-N1==0) {
        glPushMatrix();
        glMultMatrixd(cubic[i].data());
        drawRawCube();
        putStickers(i);
        glPopMatrix();
    }
}

void MachineViewer::drawRawCube() {
    const std::array<GLfloat, 9> AA = {0, 0, 0.480, 0.470, 0.465, 0.460, 0.455, 0.450, 0.450};
    GLfloat ll = 0.50;
    GLfloat aa = AA[N];

    // black plastic
    GLfloat black_ambient[]  = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat black_diffuse[]  = {0.1f, 0.1f, 0.1f, 1.0f};
    GLfloat black_specular[] = {0.7f, 0.7f, 0.7f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   black_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   black_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  black_specular);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 70.4f);
    glColor3fv(black_diffuse);

    // Faces of cube
    glBegin(GL_QUADS);
    glNormal3f( 0.,  0.,  1.);
    glVertex3f(-aa, -aa,  ll);
    glVertex3f( aa, -aa,  ll);
    glVertex3f( aa,  aa,  ll);
    glVertex3f(-aa,  aa,  ll);

    glNormal3f( 0.,  0., -1.);
    glVertex3f(-aa,  aa, -ll);
    glVertex3f( aa,  aa, -ll);
    glVertex3f( aa, -aa, -ll);
    glVertex3f(-aa, -aa, -ll);

    glNormal3f(-1.,  0.,  0.);
    glVertex3f(-ll, -aa,  aa);
    glVertex3f(-ll,  aa,  aa);
    glVertex3f(-ll,  aa, -aa);
    glVertex3f(-ll, -aa, -aa);

    glNormal3f( 1.,  0.,  0.);
    glVertex3f( ll, -aa, -aa);
    glVertex3f( ll,  aa, -aa);
    glVertex3f( ll,  aa,  aa);
    glVertex3f( ll, -aa,  aa);

    glNormal3f( 0., -1.,  0.);
    glVertex3f( aa, -ll, -aa);
    glVertex3f( aa, -ll,  aa);
    glVertex3f(-aa, -ll,  aa);
    glVertex3f(-aa, -ll, -aa);

    glNormal3f( 0.,  1.,  0.);
    glVertex3f(-aa,  ll, -aa);
    glVertex3f(-aa,  ll,  aa);
    glVertex3f( aa,  ll,  aa);
    glVertex3f( aa,  ll, -aa);
    glEnd();

    // Edges of cube
    glBegin(GL_QUADS);
    glNormal3f(-1., -1.,  0.);
    glVertex3f(-aa, -ll, -aa);
    glVertex3f(-aa, -ll,  aa);
    glVertex3f(-ll, -aa,  aa);
    glVertex3f(-ll, -aa, -aa);
    
    glNormal3f( 1.,  1.,  0.);
    glVertex3f( aa,  ll, -aa);
    glVertex3f( aa,  ll,  aa);
    glVertex3f( ll,  aa,  aa);
    glVertex3f( ll,  aa, -aa);

    glNormal3f(-1.,  1.,  0.);
    glVertex3f(-ll,  aa, -aa);
    glVertex3f(-ll,  aa,  aa);
    glVertex3f(-aa,  ll,  aa);
    glVertex3f(-aa,  ll, -aa);

    glNormal3f( 1., -1.,  0.);
    glVertex3f( ll, -aa, -aa);
    glVertex3f( ll, -aa,  aa);
    glVertex3f( aa, -ll,  aa);
    glVertex3f( aa, -ll, -aa);

    glNormal3f( 0., -1., -1.);
    glVertex3f(-aa, -aa, -ll);
    glVertex3f( aa, -aa, -ll);
    glVertex3f( aa, -ll, -aa);
    glVertex3f(-aa, -ll, -aa);

    glNormal3f( 0.,  1.,  1.);
    glVertex3f(-aa,  aa,  ll);
    glVertex3f( aa,  aa,  ll);
    glVertex3f( aa,  ll,  aa);
    glVertex3f(-aa,  ll,  aa);

    glNormal3f( 0., -1.,  1.);
    glVertex3f(-aa, -ll,  aa);
    glVertex3f( aa, -ll,  aa);
    glVertex3f( aa, -aa,  ll);
    glVertex3f(-aa, -aa,  ll);

    glNormal3f( 0.,  1., -1.);
    glVertex3f(-aa,  ll, -aa);
    glVertex3f( aa,  ll, -aa);
    glVertex3f( aa,  aa, -ll);
    glVertex3f(-aa,  aa, -ll);

    glNormal3f(-1.,  0., -1.);
    glVertex3f(-ll, -aa, -aa);
    glVertex3f(-ll,  aa, -aa);
    glVertex3f(-aa,  aa, -ll);
    glVertex3f(-aa, -aa, -ll);

    glNormal3f( 1.,  0.,  1.);
    glVertex3f( ll, -aa,  aa);
    glVertex3f( ll,  aa,  aa);
    glVertex3f( aa,  aa,  ll);
    glVertex3f( aa, -aa,  ll);

    glNormal3f( 1.,  0., -1.);
    glVertex3f( aa, -aa, -ll);
    glVertex3f( aa,  aa, -ll);
    glVertex3f( ll,  aa, -aa);
    glVertex3f( ll, -aa, -aa);

    glNormal3f(-1.,  0.,  1.);
    glVertex3f(-aa, -aa,  ll);
    glVertex3f(-aa,  aa,  ll);
    glVertex3f(-ll,  aa,  aa);
    glVertex3f(-ll, -aa,  aa);
    glEnd();

    // Corners of cube
    glBegin(GL_TRIANGLES);
    glNormal3f( 1.,  1.,  1.);
    glVertex3f( aa,  aa,  ll);
    glVertex3f( ll,  aa,  aa);
    glVertex3f( aa,  ll,  aa);

    glNormal3f(-1., -1., -1.);
    glVertex3f(-aa, -ll, -aa);
    glVertex3f(-ll, -aa, -aa);
    glVertex3f(-aa, -aa, -ll);

    glNormal3f(-1.,  1.,  1.);
    glVertex3f(-aa,  aa,  ll);
    glVertex3f(-aa,  ll,  aa);
    glVertex3f(-ll,  aa,  aa);

    glNormal3f( 1., -1., -1.);
    glVertex3f( ll, -aa, -aa);
    glVertex3f( aa, -ll, -aa);
    glVertex3f( aa, -aa, -ll);

    glNormal3f( 1., -1.,  1.);
    glVertex3f( aa, -aa,  ll);
    glVertex3f( aa, -ll,  aa);
    glVertex3f( ll, -aa,  aa);

    glNormal3f(-1.,  1., -1.);
    glVertex3f(-ll,  aa, -aa);
    glVertex3f(-aa,  ll, -aa);
    glVertex3f(-aa,  aa, -ll);

    glNormal3f(-1., -1.,  1.);
    glVertex3f(-aa, -aa,  ll);
    glVertex3f(-ll, -aa,  aa);
    glVertex3f(-aa, -ll,  aa);

    glNormal3f( 1.,  1., -1.);
    glVertex3f( ll,  aa, -aa);
    glVertex3f( aa,  aa, -ll);
    glVertex3f( aa,  ll, -aa);
    glEnd();
}

void MachineViewer::putStickers(int i) {
    GLfloat EE[9] = {0, 0, 0.460, 0.425, 0.415, 0.405, 0.400, 0.395, 0.390};
    GLfloat dd = (N<4) ? 0.505 : 0.510;
    GLfloat ee = EE[N];
    GLfloat ff = ee - ((N==2)?.02:.04);
    int j=(i%N), k=(i%(N*N))/N, l=(i/(N*N));

    glEnable(GL_COLOR_MATERIAL);

    if (j==0) { // B
        // yellow plastic
        GLfloat yellow_ambient[]  = {0.2f, 0.2f, 0.0f, 1.0f};
        GLfloat yellow_diffuse[]  = {0.7f, 0.7f, 0.0f, 1.0f};
        GLfloat yellow_specular[] = {0.8f, 0.8f, 0.8f, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   yellow_ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   yellow_diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  yellow_specular);
        glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 70.4f);
        glColor3fv(yellow_diffuse);
        glBegin(GL_POLYGON);
        glNormal3f(-1.,  0.,  0.);
        glVertex3f(-dd, -ff,  ee);
        glVertex3f(-dd,  ff,  ee);
        glVertex3f(-dd,  ee,  ff);
        glVertex3f(-dd,  ee, -ff);
        glVertex3f(-dd,  ff, -ee);
        glVertex3f(-dd, -ff, -ee);
        glVertex3f(-dd, -ee, -ff);
        glVertex3f(-dd, -ee,  ff);
        glEnd();
    }
    if (j==N-1) { // F
        // white plastic
        GLfloat white_ambient[]  = {0.2f, 0.2f, 0.2f, 1.0f};
        GLfloat white_diffuse[]  = {0.7f, 0.7f, 0.8f, 1.0f};
        GLfloat white_specular[] = {0.8f, 0.8f, 0.8f, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   white_ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   white_diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  white_specular);
        glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 70.4f);
        glColor3fv(white_diffuse);
        glBegin(GL_POLYGON);
        glNormal3f( 1.,  0.,  0.);
        glVertex3f( dd, -ff, -ee);
        glVertex3f( dd,  ff, -ee);
        glVertex3f( dd,  ee, -ff);
        glVertex3f( dd,  ee,  ff);
        glVertex3f( dd,  ff,  ee);
        glVertex3f( dd, -ff,  ee);
        glVertex3f( dd, -ee,  ff);
        glVertex3f( dd, -ee, -ff);
        glEnd();
    }
    if (k==0) { // L
        // orange plastic
        GLfloat orange_ambient[]  = {0.2f, 0.0f, 0.0f, 1.0f};
        GLfloat orange_diffuse[]  = {0.8f, 0.5f, 0.0f, 1.0f};
        GLfloat orange_specular[] = {0.8f, 0.8f, 0.7f, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   orange_ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   orange_diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  orange_specular);
        glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 70.4f);
        glColor3fv(orange_diffuse);
        glBegin(GL_POLYGON);
        glNormal3f( 0., -1.,  0.);
        glVertex3f( ee, -dd, -ff);
        glVertex3f( ee, -dd,  ff);
        glVertex3f( ff, -dd,  ee);
        glVertex3f(-ff, -dd,  ee);
        glVertex3f(-ee, -dd,  ff);
        glVertex3f(-ee, -dd, -ff);
        glVertex3f(-ff, -dd, -ee);
        glVertex3f( ff, -dd, -ee);
        glEnd();
    }
    if (k==N-1) { // R
        // red plastic
        GLfloat red_ambient[]  = {0.0f, 0.0f, 0.0f, 1.0f};
        GLfloat red_diffuse[]  = {0.7f, 0.0f, 0.0f, 1.0f};
        GLfloat red_specular[] = {0.8f, 0.8f, 0.8f, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   red_ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   red_diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  red_specular);
        glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 70.4f);
        glColor3fv(red_diffuse);
        glBegin(GL_POLYGON);
        glNormal3f( 0.,  1.,  0.);
        glVertex3f(-ee,  dd, -ff);
        glVertex3f(-ee,  dd,  ff);
        glVertex3f(-ff,  dd,  ee);
        glVertex3f( ff,  dd,  ee);
        glVertex3f( ee,  dd,  ff);
        glVertex3f( ee,  dd, -ff);
        glVertex3f( ff,  dd, -ee);
        glVertex3f(-ff,  dd, -ee);
        glEnd();
    }
    if (l==0) { // D
        // green plastic
        GLfloat green_ambient[]  = {0.0f, 0.1f, 0.0f, 1.0f};
        GLfloat green_diffuse[]  = {0.0f, 0.4f, 0.1f, 1.0f};
        GLfloat green_specular[] = {0.8f, 0.8f, 0.8f, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   green_ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   green_diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  green_specular);
        glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 70.4f);
        glColor3fv(green_diffuse);
        glBegin(GL_POLYGON);
        glNormal3f( 0.,  0., -1.);
        glVertex3f(-ff,  ee, -dd);
        glVertex3f( ff,  ee, -dd);
        glVertex3f( ee,  ff, -dd);
        glVertex3f( ee, -ff, -dd);
        glVertex3f( ff, -ee, -dd);
        glVertex3f(-ff, -ee, -dd);
        glVertex3f(-ee, -ff, -dd);
        glVertex3f(-ee,  ff, -dd);
        glEnd();
    }
    if (l==N-1) { // U
        // blue plastic
        GLfloat blue_ambient[]  = {0.0f, 0.0f, 0.1f, 1.0f};
        GLfloat blue_diffuse[]  = {0.0f, 0.0f, 0.6f, 1.0f};
        GLfloat blue_specular[] = {0.8f, 0.8f, 0.8f, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   blue_ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   blue_diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  blue_specular);
        glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 70.4f);
        glColor3fv(blue_diffuse);
        glBegin(GL_POLYGON);
        glNormal3f( 0.,  0.,  1.);
        glVertex3f(-ff, -ee,  dd);
        glVertex3f( ff, -ee,  dd);
        glVertex3f( ee, -ff,  dd);
        glVertex3f( ee,  ff,  dd);
        glVertex3f( ff,  ee,  dd);
        glVertex3f(-ff,  ee,  dd);
        glVertex3f(-ee,  ff,  dd);
        glVertex3f(-ee, -ff,  dd);
        glEnd();
    }
}

void MachineViewer::rotSlices() {
    if (tmp_act.mask == 0) {
        return;
    }
    for (int i = 0; i < N*N*N; ++i) {
        int axis_row = std::abs(tmp_act.axis) - 1;
        slice[i] = static_cast<uint8_t>(cubic[i](axis_row, 3) + 0.5 * (N - 1));
    }

    det_act = tmp_act;
    tmp_act.clear();
    prev_cubic = cubic;
    slice_angle = dangle;
    rotating = true;
}

void MachineViewer::rotRandom() {
    const std::array<uint8_t, 8> rand_mask = {1, 2, 4, 3, 6, 8, 12, 14};
    const std::array<int, 6> rand_axis = {1, -1, 2, -2, 3, -3};
    std::random_device rnd;
    std::mt19937 mt(rnd());
    std::uniform_int_distribution<> rand6(0, 5);
    std::uniform_int_distribution<> rand8(0, 7);
    tmp_act.axis = rand_axis[rand6(mt)];
    tmp_act.mask = rand_mask[rand8(mt)];
    rotSlices();
}

void MachineViewer::makeMotionsFromSolution(std::string& solution, std::string& init, std::string& solve) {
    std::istringstream iss(solution);
    std::string sub_str;
    std::vector<std::string> str_vec;
    while (iss >> sub_str) {
        solve += dict.at(sub_str);
        str_vec.push_back(sub_str);
    }
    for (auto it = str_vec.rbegin(); it != str_vec.rend(); ++it) {
        init += inverse_dict.at(*it);
    }
}

bool MachineViewer::makeAction(int key) {
    bool determined = false;
    switch (key) {
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':{
            tmp_act.mask |= (1 << (key-'1'));
            break;
        }
        case 'j':
        case 'J':{
            tmp_act.axis = 1;
            determined = true;
            break;
        }
        case 'k':
        case 'K':{
            tmp_act.axis = 2;
            determined = true;
            break;
        }
        case 'l':
        case 'L':{
            tmp_act.axis = 3;
            determined = true;
            break;
        }
        case 'u':
        case 'U':{
            tmp_act.axis = -1;
            determined = true;
            break;
        }
        case 'i':
        case 'I':{
            tmp_act.axis = -2;
            determined = true;
            break;
        }
        case 'o':
        case 'O':{
            tmp_act.axis = -3;
            determined = true;
            break;
        }
    }

    return determined;
}

void MachineViewer::makeTrans() {
    if ((!rotating) & solve_motion_it != solve_motion.end()) {
        if (makeAction(*solve_motion_it)) {
            rotSlices();
        }
        solve_motion_it++;
    }
    else if ((!rotating) & (scramble > 0)) {
        rotRandom();
        scramble--;
    }
    if (auto_rotate) {
        glRotatef(1, 0, 1, 0);
    }
}