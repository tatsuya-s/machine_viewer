# ifdef _WIN32
#   include <windows.h>
# endif
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "glm.h"

#define bool int
#define true 1
#define false 0

double _left = 0.0;
double _right = 0.0;
double _bottom = 0.0;
double _top = 0.0;
double _zNear = 0.1;
double _zFar = 10.0;
double fovy = 45.0;
double prev_z = 0;

int _mouseX = 0;
int _mouseY = 0;
bool _mouseLeft = false;
bool _mouseMiddle = false;
bool _mouseRight = false;
double _dragPosX = 0.0;
double _dragPosY = 0.0;
double _dragPosZ = 0.0;

double _matrix[16];
double _matrixI[16];
double vlen(double x, double y, double z);
void pos(double *px, double *py, double *pz, const int x, const int y, const int *viewport);
void getMatrix();
void invertMatrix(const GLdouble * m, GLdouble * out);

int full_screen = 0;

GLMmodel *main_model = NULL;
GLMmodel *bc_l_model = NULL;
GLMmodel *bc_r_model = NULL;
GLMmodel *cube_model = NULL;

int ww, wh;
int show_axis = 1;
int show_help = 0;
int benchmark = 0;
float auto_rotate = 0;
int xrotate = 0;
int yrotate = 1;
int zrotate = 0;

int lighting = 1;

void *Help_Font = GLUT_BITMAP_8_BY_13;
int linestart = 10;
int linespace = 20;

/* Lighting Variables */
GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

GLfloat mat_ambient[] = { 0.7, 0.7, 0.7, 1.0 };
GLfloat mat_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat high_shininess[] = { 100.0 };

void Reshape(int w, int h);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void Keyboard(unsigned char key, int x, int y);
void Display(void);
void AutoSpin(void);
void DrawModel(GLMmodel *pmodel);
void DrawAxis(float scale);
void HelpDisplay(GLint ww, GLint wh);
void HelpRenderBitmapString(float x, float y, void *font, char *string);

int main(int argc, char **argv)
{
    if (argc > 1) {    
        for (int i = 0; i < argc; i++) {
            if (strstr(argv[i], "-f")) {
                full_screen = 1;
            }
        }
    } 
    else {
        printf("Usage: %s [-f] <obj filename>\n", argv[0]);
        exit(0);
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(1000, 1000);
    glutCreateWindow("Machine Viewer");
    if (full_screen) {
        glutFullScreen();
    }

    glutDisplayFunc(Display);
    glutKeyboardFunc(Keyboard);
    getMatrix();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearAccum(0.0, 0.0, 0.0, 0.0);
    glutReshapeFunc(Reshape);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    glutIdleFunc(NULL);

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

    main_model = glmReadOBJ("../obj/main.obj");
    bc_l_model = glmReadOBJ("../obj/bc_l_body.obj");
    bc_r_model = glmReadOBJ("../obj/bc_r_body.obj");
    cube_model = glmReadOBJ("../obj/cube_625.obj");
    //glmUnitize(pmodel);
    //glmVertexNormals(pmodel, 90.0, GL_TRUE);

    glutMainLoop();

    return 0;
}

void Reshape(int w, int h)
{
    glViewport(0, 0, w, h);

    ww = w;
    wh = h;

    _top = 1.0;
    _bottom = -1.0;
    _left = -(double) w / (double) h;
    _right = -_left;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovy, (double) w / (double) h, _zNear, _zFar);

    glMatrixMode(GL_MODELVIEW);
}

void Mouse(int button, int state, int x, int y)
{
    int viewport[4];

    _mouseX = x;
    _mouseY = y;

    if (state == GLUT_UP)
        switch (button) {
        case GLUT_LEFT_BUTTON:
            _mouseLeft = false;
            break;
        case GLUT_MIDDLE_BUTTON:
            _mouseMiddle = false;
            break;
        case GLUT_RIGHT_BUTTON:
            _mouseRight = false;
            break;
    } else
    switch (button) {
        case GLUT_LEFT_BUTTON:
            _mouseLeft = true;
            break;
        case GLUT_MIDDLE_BUTTON:
            _mouseMiddle = true;
            break;
        case GLUT_RIGHT_BUTTON:
            _mouseRight = true;
            break;
    }

    glGetIntegerv(GL_VIEWPORT, viewport);
    pos(&_dragPosX, &_dragPosY, &_dragPosZ, x, y, viewport);
}

void Motion(int x, int y)
{
    bool changed = false;

    const int dx = x - _mouseX;
    const int dy = y - _mouseY;

    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    if (dx == 0 && dy == 0) {
        return;
    }

    if (_mouseMiddle || (_mouseLeft && _mouseRight)) {
        glLoadIdentity();
        glTranslatef(0, 0, dy * 0.01);
        glMultMatrixd(_matrix);
        changed = true;
    } 
    else if (_mouseLeft) {
        double ax, ay, az;
        double bx, by, bz;
        double angle;

        ax = dy;
        ay = dx;
        az = 0.0;
        angle = vlen(ax, ay, az) / (double) (viewport[2] + 1) * 180.0;

        bx = _matrixI[0] * ax + _matrixI[4] * ay + _matrixI[8] * az;
        by = _matrixI[1] * ax + _matrixI[5] * ay + _matrixI[9] * az;
        bz = _matrixI[2] * ax + _matrixI[6] * ay + _matrixI[10] * az;

        glRotatef(angle, bx, by, bz);

        changed = true;
    } 
    else if (_mouseRight) {
        double px, py, pz;

        pos(&px, &py, &pz, x, y, viewport);

        glLoadIdentity();
        glTranslatef(px - _dragPosX, py - _dragPosY, pz - _dragPosZ);
        glMultMatrixd(_matrix);

        _dragPosX = px;
        _dragPosY = py;
        _dragPosZ = pz;

        changed = true;
    }

    _mouseX = x;
    _mouseY = y;

    if (changed) {
        getMatrix();
        glutPostRedisplay();
    }
}

void AutoSpin(void)
{
    if (xrotate || yrotate || zrotate) {
        glRotatef(1, xrotate, yrotate, zrotate);
        getMatrix();
        glutPostRedisplay();
    }
}

void Keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 'h':
    case 'H':{
        show_help = !show_help;
        break;
    }
    case 'r':
    case 'R':{
        for (int i = 0; i < 16; i++) {
            if (i == 0 || i == 5 || i == 10 || i == 15) {
                _matrix[i] = 1;
            } else {
                _matrix[i] = 0;
            }
        }
        glLoadIdentity();
        glMultMatrixd(_matrix);
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
        benchmark = !benchmark;
        if (benchmark) {
            glutIdleFunc(AutoSpin);
        }
        else {
            glutIdleFunc(NULL);
        }
        break;
    }
    case 'x':
    case 'X':
    {
        xrotate = !xrotate;
        break;
    }
    case 'y':
    case 'Y':
    {
        yrotate = !yrotate;
        break;
    }
    case 'z':
    case 'Z':
    {
        zrotate = !zrotate;
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
        exit(0);
        break;
    }
    default:{
        break;
    }
    }
    glutPostRedisplay();
}

double vlen(double x, double y, double z)
{
    return sqrt(x * x + y * y + z * z);
}

void pos(double *px, double *py, double *pz, const int x, const int y, const int *viewport)
{
    *px = (double) (x - viewport[0]) / (double) (viewport[2]);
    *py = (double) (y - viewport[1]) / (double) (viewport[3]);

    *px = _left + (*px) * (_right - _left);
    *py = _top + (*py) * (_bottom - _top);
    *pz = _zNear;
}

void getMatrix()
{
    glGetDoublev(GL_MODELVIEW_MATRIX, _matrix);
    invertMatrix(_matrix, _matrixI);
}

void invertMatrix(const GLdouble * m, GLdouble * out)
{
    #define MAT(m,r,c) (m)[(c)*4+(r)]

    GLdouble det;
    GLdouble d12, d13, d23, d24, d34, d41;
    GLdouble tmp[16];
    d12 = (MAT(m,2,0) * MAT(m,3,1) - MAT(m,3,0) * MAT(m,2,1));
    d13 = (MAT(m,2,0) * MAT(m,3,2) - MAT(m,3,0) * MAT(m,2,2));
    d23 = (MAT(m,2,1) * MAT(m,3,2) - MAT(m,3,1) * MAT(m,2,2));
    d24 = (MAT(m,2,1) * MAT(m,3,3) - MAT(m,3,1) * MAT(m,2,3));
    d34 = (MAT(m,2,2) * MAT(m,3,3) - MAT(m,3,2) * MAT(m,2,3));
    d41 = (MAT(m,2,3) * MAT(m,3,0) - MAT(m,3,3) * MAT(m,2,0));

    tmp[0] =  (MAT(m,1,1) * d34 - MAT(m,1,2) * d24 + MAT(m,1,3) * d23);
    tmp[1] = -(MAT(m,1,0) * d34 + MAT(m,1,2) * d41 + MAT(m,1,3) * d13);
    tmp[2] =  (MAT(m,1,0) * d24 + MAT(m,1,1) * d41 + MAT(m,1,3) * d12);
    tmp[3] = -(MAT(m,1,0) * d23 - MAT(m,1,1) * d13 + MAT(m,1,2) * d12);

    det = MAT(m,0,0) * tmp[0] + MAT(m,0,1) * tmp[1] + MAT(m,0,2) * tmp[2] + MAT(m,0,3) * tmp[3];

    if (det > 0.0) {
        GLdouble invDet = 1.0 / det;
        tmp[0] *= invDet;
        tmp[1] *= invDet;
        tmp[2] *= invDet;
        tmp[3] *= invDet;

        tmp[4] = -(MAT(m,0,1) * d34 - MAT(m,0,2) * d24 + MAT(m,0,3) * d23) * invDet;
        tmp[5] =  (MAT(m,0,0) * d34 + MAT(m,0,2) * d41 + MAT(m,0,3) * d13) * invDet;
        tmp[6] = -(MAT(m,0,0) * d24 + MAT(m,0,1) * d41 + MAT(m,0,3) * d12) * invDet;
        tmp[7] =  (MAT(m,0,0) * d23 - MAT(m,0,1) * d13 + MAT(m,0,2) * d12) * invDet;

        d12 = MAT(m,0,0) * MAT(m,1,1) - MAT(m,1,0) * MAT(m,0,1);
        d13 = MAT(m,0,0) * MAT(m,1,2) - MAT(m,1,0) * MAT(m,0,2);
        d23 = MAT(m,0,1) * MAT(m,1,2) - MAT(m,1,1) * MAT(m,0,2);
        d24 = MAT(m,0,1) * MAT(m,1,3) - MAT(m,1,1) * MAT(m,0,3);
        d34 = MAT(m,0,2) * MAT(m,1,3) - MAT(m,1,2) * MAT(m,0,3);
        d41 = MAT(m,0,3) * MAT(m,1,0) - MAT(m,1,3) * MAT(m,0,0);

        tmp[8]  =  (MAT(m,3,1) * d34 - MAT(m,3,2) * d24 + MAT(m,3,3) * d23) * invDet;
        tmp[9]  = -(MAT(m,3,0) * d34 + MAT(m,3,2) * d41 + MAT(m,3,3) * d13) * invDet;
        tmp[10] =  (MAT(m,3,0) * d24 + MAT(m,3,1) * d41 + MAT(m,3,3) * d12) * invDet;
        tmp[11] = -(MAT(m,3,0) * d23 - MAT(m,3,1) * d13 + MAT(m,3,2) * d12) * invDet;
        tmp[12] = -(MAT(m,2,1) * d34 - MAT(m,2,2) * d24 + MAT(m,2,3) * d23) * invDet;
        tmp[13] =  (MAT(m,2,0) * d34 + MAT(m,2,2) * d41 + MAT(m,2,3) * d13) * invDet;
        tmp[14] = -(MAT(m,2,0) * d24 + MAT(m,2,1) * d41 + MAT(m,2,3) * d12) * invDet;
        tmp[15] =  (MAT(m,2,0) * d23 - MAT(m,2,1) * d13 + MAT(m,2,2) * d12) * invDet;

        memcpy(out, tmp, 16 * sizeof(GLdouble));
    }

    #undef MAT
}

void DrawModel(GLMmodel *pmodel)
{
    GLuint mode = 0;
    mode = GLM_NONE;
    mode = mode | GLM_FLAT;
    mode = mode | GLM_COLOR; //mode = mode | GLM_MATERIAL;

    glPushMatrix();
    if (pmodel) {
        glScalef(0.001, 0.001, 0.001);
        glmDraw(pmodel, mode);
    }
    glPopMatrix();
}

void DrawAxis(float scale)
{
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glScalef(scale, scale, scale);

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

void HelpDisplay(GLint ww, GLint wh)
{
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, ww, 0, wh);
    glScalef(1, -1, 1);
    glTranslatef(0, -wh, 0);
    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
    glLoadIdentity();
    linestart = 10;

    HelpRenderBitmapString(30, linestart +=
               linespace, Help_Font, "Help Menu");
    HelpRenderBitmapString(30, linestart +=
               linespace, Help_Font, "---------");
    HelpRenderBitmapString(30, linestart +=
               linespace, Help_Font,
               "H/h = Toggle Help Menu");
    if (!full_screen)
    HelpRenderBitmapString(30, linestart +=
                   linespace, Help_Font,
                   "TAB = Activate Full Screen");
    HelpRenderBitmapString(30, linestart +=
               linespace, Help_Font,
               "Esc = Exits Program");
    HelpRenderBitmapString(30, linestart +=
               linespace, Help_Font,
               "R/r = Reset Position");
    HelpRenderBitmapString(30, linestart +=
               linespace, Help_Font,
               "B/b = Toggle Auto Rotate");

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    if (lighting) {
        glEnable(GL_LIGHTING);
    }
}

void HelpRenderBitmapString(float x, float y, void *font, char *string)
{
    char *c;
    glRasterPos2f(x, y);
    for (c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

void Display(void)
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
    glTranslatef(0, 0, -1);
    glMultMatrixd(_matrix);

    if (show_axis) {
        DrawAxis(0.065); // 65mm
    }
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    DrawModel(main_model);
    DrawModel(bc_l_model);
    DrawModel(bc_r_model);
    DrawModel(cube_model);
    glPopMatrix();

    if (show_help) {
        HelpDisplay(ww, wh);
    }

    glutSwapBuffers();
}
