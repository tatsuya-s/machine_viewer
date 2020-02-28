#include "cube.hpp"

int show_axis     = true;
int auto_rotate   = false;
int lighting      = true;
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

const GLfloat light_ambient[]  = { 0.0, 0.0, 0.0, 1.0 };
const GLfloat light_diffuse[]  = { 1.0, 1.0, 1.0, 1.0 };
const GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
const GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
const GLfloat mat_ambient[]    = { 0.7, 0.7, 0.7, 1.0 };
const GLfloat mat_diffuse[]    = { 0.8, 0.8, 0.8, 1.0 };
const GLfloat mat_specular[]   = { 1.0, 1.0, 1.0, 1.0 };
const GLfloat high_shininess[] = { 100.0 };

typedef GLfloat Matrix[16];

typedef struct {
    Matrix M;
} Cubic;

typedef struct {
    GLubyte mask; // 8-bit mask: selected slices
    GLbyte  axis; // 0, +-1(x), +-2(y), +-3(z): rotation axis
} Action;

typedef struct {
    GLfloat x, y, z;
} Vector;

#define MAXSIZE 8
Cubic cubic[ MAXSIZE * MAXSIZE * MAXSIZE ];

#define STACKSIZE 10000
Action  act[ STACKSIZE ], *asp = act;

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(1000, 1000);
    glutCreateWindow("Machine Viewer");
    if (full_screen) {
        glutFullScreen();
    }

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    getMatrix();
    glClearColor(0.05f, 0.1f, 0.15f, 1.0f);
    glClearAccum(0.0, 0.0, 0.0, 0.0);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutIdleFunc(nullptr);

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

    if (lighting) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
    }
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

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
    for (const auto& file : files) {
        GLMmodel* model = glmReadOBJ(file.c_str());
        models.push_back(std::unique_ptr<GLMmodel>(model));
    }

    initCubic(4);

    glutMainLoop();

    return 0;
}

void reshape(int w, int h)
{
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

void mouse(int button, int state, int x, int y)
{
    Eigen::Vector4i viewport;

    mouse_x = x;
    mouse_y = y;

    if (state == GLUT_UP)
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
    } else
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

    glGetIntegerv(GL_VIEWPORT, viewport.data());
    getDragPos(drag_x, drag_y, drag_z, x, y, viewport);
}

void motion(int x, int y)
{
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

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 'r':
    case 'R':{
        modelview_matrix = Eigen::MatrixXd::Identity(4, 4);
        glLoadIdentity();
        glMultMatrixd(modelview_matrix.data());
        getMatrix();
        break;
    }
    case 'a':
    case 'A':{
        show_axis = !show_axis;
        break;
    }
    case 'b':
    case 'B':
    {
        auto_rotate = !auto_rotate;
        if (auto_rotate) {
            glutIdleFunc(autoSpin);
        }
        else {
            glutIdleFunc(nullptr);
        }
        break;
    }
    case 'l':
    case 'L':
        lighting = !lighting;
        break;
    case 9:{
        if (!full_screen) {
            glutFullScreen();
            full_screen = 1;
        }
        break;
    }
    case 'q':
    case 'Q':
    case 27:{
        std::exit(EXIT_SUCCESS);
        break;
    }
    default:{
        break;
    }
    }
    glutPostRedisplay();
}

void display(void)
{
    if (lighting) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
    } else {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();

    glLoadIdentity();
    glTranslatef(0, 0, -0.5);
    glMultMatrixd(modelview_matrix.data());

    if (show_axis) {
        drawAxis(0.065); // 65mm
    }
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    for (const auto& model : models) {
        drawModel(model);
    }

    drawCubic(4);

    glPopMatrix();

    glutSwapBuffers();
}

void autoSpin(void)
{
    glRotatef(1, 0, 1, 0);
    getMatrix();
    glutPostRedisplay();
}

void drawAxis(float scale)
{
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glScalef(scale, scale, scale);
    glLineWidth(3.0);

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
    if (lighting) {
        glEnable(GL_LIGHTING);
    }
    glColor3f(1.0, 1.0, 1.0);
    glPopMatrix();
}

void drawModel(const std::unique_ptr<GLMmodel>& model)
{
    glPushMatrix();
    glScalef(0.001, 0.001, 0.001);
    glmDraw(model.get(), GLM_FLAT | GLM_COLOR);
    glPopMatrix();
}

void getDragPos(double& px, double& py, double& pz, const int x, const int y, const Eigen::Vector4i& viewport)
{
    px = (double) (x - viewport(0)) / (double) (viewport(2));
    py = (double) (y - viewport(1)) / (double) (viewport(3));

    px = left + px * (right - left);
    py = top + py * (bottom - top);
    pz = near;
}

void getMatrix()
{
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview_matrix.data());
}











void initCubic(int N)
{
    //Eigen::Matrix4d E = Eigen::MatrixXd::Identity(4, 4);
    Matrix E = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    for (int i = 0; i < N*N*N; i++) {
        memcpy(cubic[i].M, E, sizeof(Matrix)); 
        cubic[i].M[12] = (i%N)       - .5*(N-1);
        cubic[i].M[13] = (i%(N*N))/N - .5*(N-1);
        cubic[i].M[14] = (i/(N*N))   - .5*(N-1);
    }
    asp = act;
}

void drawCubic(int N)
{
    int i = N*N*N;
    glPushMatrix();
    glScalef(0.0156, 0.0156, 0.0156); // 4 x 0.015625 = 0.0625
    while (--i >= 0) {
        drawCube(N, i);
    }
    glPopMatrix();
}

void drawCube(int N, int i)
{
    int N1 = N-1;
    int j = (i%N), k = (i%(N*N))/N, l = (i/(N*N));
    if (j==0 || j-N1==0 || k==0 || k-N1==0 || l==0 || l-N1==0) {
        glPushMatrix();
        glMultMatrixf(cubic[i].M);
        drawRawCube(N);
        putStickers(i, N);
        glPopMatrix();
    }
}

void drawRawCube(int N)
{
    GLfloat AA[9] = {0, 0, 0.480, 0.470, 0.465, 0.460, 0.455, 0.450, 0.450};
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

void putStickers(int i, int N)
{
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