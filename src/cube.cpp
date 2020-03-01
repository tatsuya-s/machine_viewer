#include "cube.hpp"

int auto_rotate   = false;
int full_screen   = false;

double left       = 0.0;
double right      = 0.0;
double bottom     = 0.0;
double top        = 0.0;
double near       = 0.1;
double far        = 10.0;
double fovy       = 45.0;

int mouse_x       = 0;
int mouse_y       = 0;
bool mouse_left   = false;
bool mouse_middle = false;
bool mouse_right  = false;
double drag_x     = 0.0;
double drag_y     = 0.0;
double drag_z     = 0.0;

Eigen::Matrix4d modelview_matrix;

std::vector<std::unique_ptr<GLMmodel>> models;

static constexpr std::array<float, 4> light_ambient  = { 0.0, 0.0, 0.0, 1.0 };
static constexpr std::array<float, 4> light_diffuse  = { 1.0, 1.0, 1.0, 1.0 };
static constexpr std::array<float, 4> light_specular = { 1.0, 1.0, 1.0, 1.0 };
static constexpr std::array<float, 4> light_position = { 1.0, 1.0, 1.0, 0.0 };
static constexpr std::array<float, 4> mat_ambient    = { 0.7, 0.7, 0.7, 1.0 };
static constexpr std::array<float, 4> mat_diffuse    = { 0.8, 0.8, 0.8, 1.0 };
static constexpr std::array<float, 4> mat_specular   = { 1.0, 1.0, 1.0, 1.0 };
static constexpr std::array<float, 1> high_shininess = { 100.0 };

static constexpr int N = 4;
std::array<Eigen::Matrix4d, N*N*N> cubic, prev_cubic;
std::array<uint8_t, N*N*N> slice;
Action tmp_act, det_act;
double dangle = 90.0 / 10;
double slice_angle = 0;
int scramble = 0;
bool rotating = false;
bool skip_anim = false;

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

std::string::iterator solve_motion_it;
std::string solve_motion;

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(800, 800);
    glutCreateWindow("Machine Viewer");
    if (full_screen) {
        glutFullScreen();
    }

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(60, onTimer, 60);
    getMatrix();
    glClearColor(0.05f, 0.1f, 0.15f, 1.0f);
    glClearAccum(0.0, 0.0, 0.0, 0.0);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

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

    std::vector<std::string> files;
    files.push_back("../obj/main.obj");
    files.push_back("../obj/bc_l_body.obj");
    files.push_back("../obj/bc_l_tl.obj");
    files.push_back("../obj/bc_l_tr.obj");
    files.push_back("../obj/bc_l_bl.obj");
    //files.push_back("../obj/bc_l_br.obj");//
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

    std::string solution;
    bool is_first = true;
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            if (!is_first) {
                solution += " ";
            }
            else {
                is_first = false;
            }
            solution += argv[i];
        }
    }

    std::string init_motion;
    makeMotionsFromSolution(solution, init_motion, solve_motion);
    solve_motion_it = solve_motion.begin();

    std::cout << solution << std::endl;
    std::cout << init_motion << std::endl;
    std::cout << solve_motion << std::endl;

    initCubic(init_motion);

    glutMainLoop();

    return 0;
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);

    top = 1.0;
    bottom = -1.0;
    left = -(double) w / (double) h;
    right = -left;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovy, (double) w / (double) h, near, far);

    glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state, int x, int y) {
    Eigen::Vector4i viewport;

    mouse_x = x;
    mouse_y = y;

    if (state == GLUT_UP) {
        switch (button) {
            case GLUT_LEFT_BUTTON:
                mouse_left = false;
                break;
            case GLUT_MIDDLE_BUTTON:
                mouse_middle = false;
                break;
            case GLUT_RIGHT_BUTTON:
                mouse_right = false;
                break;
        }
    }
    else {
        switch (button) {
            case GLUT_LEFT_BUTTON:
                mouse_left = true;
                break;
            case GLUT_MIDDLE_BUTTON:
                mouse_middle = true;
                break;
            case GLUT_RIGHT_BUTTON:
                mouse_right = true;
                break;
        }
    }

    glGetIntegerv(GL_VIEWPORT, viewport.data());
    getDragPos(drag_x, drag_y, drag_z, x, y, viewport);
}

void motion(int x, int y) {
    bool changed = false;

    const int dx = x - mouse_x;
    const int dy = y - mouse_y;

    Eigen::Vector4i viewport;
    glGetIntegerv(GL_VIEWPORT, viewport.data());

    if (dx == 0 && dy == 0) {
        return;
    }

    if (mouse_middle || (mouse_left && mouse_right)) {
        glLoadIdentity();
        glTranslatef(0, 0, dy * 0.01);
        glMultMatrixd(modelview_matrix.data());
        changed = true;
    } 
    else if (mouse_left) {
        double ax, ay, az;
        double bx, by, bz;
        double angle;

        ax = dy;
        ay = dx;
        az = 0.0;
        angle = std::sqrt(ax * ax + ay * ay + az * az) / (double) (viewport(2) + 1) * 180.0;
        
        Eigen::Matrix4d inverse = modelview_matrix.inverse().matrix();
        bx = inverse(0,0) * ax + inverse(1,0) * ay + inverse(2,0) * az;
        by = inverse(0,1) * ax + inverse(1,1) * ay + inverse(2,1) * az;
        bz = inverse(0,2) * ax + inverse(1,2) * ay + inverse(2,2) * az;

        glRotatef(angle, bx, by, bz);

        changed = true;
    } 
    else if (mouse_right) {
        double px, py, pz;

        getDragPos(px, py, pz, x, y, viewport);

        glLoadIdentity();
        glTranslatef(px - drag_x, py - drag_y, pz - drag_z);
        glMultMatrixd(modelview_matrix.data());

        drag_x = px;
        drag_y = py;
        drag_z = pz;

        changed = true;
    }

    mouse_x = x;
    mouse_y = y;

    if (changed) {
        getMatrix();
        glutPostRedisplay();
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'r':{
            modelview_matrix = Eigen::MatrixXd::Identity(4, 4);
            glLoadIdentity();
            glMultMatrixd(modelview_matrix.data());
            getMatrix();
            break;
        }
        case 'b':{
            auto_rotate = !auto_rotate;
            break;
        }
        case 9:{
            if (!full_screen) {
                glutFullScreen();
                full_screen = 1;
            }
            break;
        }
        case 'q':
        case 27:{
            std::exit(EXIT_SUCCESS);
            break;
        }
        case 's':{
            scramble = 30;
            break;
        }
        default:{
            if (makeAction(key)) {
                rotSlices();
            }
            break;
        }
    }
    glutPostRedisplay();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();

    glLoadIdentity();
    glTranslatef(0, 0, -0.5);
    glRotatef( 20, 1, 0, 0);
    glRotatef(-20, 0, 1, 0);
    glRotatef( -2, 0, 0, 1);
    glMultMatrixd(modelview_matrix.data());

    drawAxis(0.055); // 55mm
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    for (const auto& model : models) {
        drawModel(model);
    }

    drawCubic();

    glPopMatrix();

    glutSwapBuffers();
}

void onTimer(int value) {
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
        getMatrix();
    }
    glutPostRedisplay();
    glutTimerFunc(value, onTimer, value);
}

void drawAxis(float scale) {
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

void drawModel(const std::unique_ptr<GLMmodel>& model) {
    glPushMatrix();
    glScalef(0.001, 0.001, 0.001);
    glmDraw(model.get(), GLM_FLAT | GLM_COLOR);
    glPopMatrix();
}

void getDragPos(double& px, double& py, double& pz, const int x, const int y, const Eigen::Vector4i& viewport) {
    px = (double) (x - viewport(0)) / (double) (viewport(2));
    py = (double) (y - viewport(1)) / (double) (viewport(3));

    px = left + px * (right - left);
    py = top + py * (bottom - top);
    pz = near;
}

void getMatrix() {
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview_matrix.data());
}

void initCubic(std::string& init) {
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

void drawCubic() {
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

void drawCube(int i) {
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

void drawRawCube() {
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

    /* Faces of cube */
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

    /* Edges of cube */
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

    /* Corners of cube */
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

void putStickers(int i) {
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

void rotSlices() {
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

void rotRandom() {
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

void makeMotionsFromSolution(std::string& solution, std::string& init, std::string& solve) {
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

bool makeAction(unsigned char key) {
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
        case 'j':{
            tmp_act.axis = 1;
            determined = true;
            break;
        }
        case 'k':{
            tmp_act.axis = 2;
            determined = true;
            break;
        }
        case 'l':{
            tmp_act.axis = 3;
            determined = true;
            break;
        }
        case 'u':{
            tmp_act.axis = -1;
            determined = true;
            break;
        }
        case 'i':{
            tmp_act.axis = -2;
            determined = true;
            break;
        }
        case 'o':{
            tmp_act.axis = -3;
            determined = true;
            break;
        }
    }

    return determined;
}