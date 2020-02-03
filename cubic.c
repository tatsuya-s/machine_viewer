/*
 * cubic.c: simulate Rubik's cube
 *         ("Rubik's cube" is trade mark of Tsukuda Original)
 *
 * 1999.3, tsakurai@rimath.saitama-u.ac.jp
 * 1999.7, add mouse-action
 * 2006.5, enable revenge, professor, etc 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <GLUT/glut.h>

void draw_raw_cube(int N)
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

void put_stickers(int i, int N)
{
    GLfloat EE[9] = {0, 0, 0.460, 0.425, 0.415, 0.405, 0.400, 0.395, 0.390};
    GLfloat dd = (N<4)? 0.505 : 0.510;
    GLfloat ee = EE[N];
    GLfloat ff = ee - ((N==2)?.02:.04);
    int j=(i%N), k=(i%(N*N))/N, l=(i/(N*N));

    if(j==0) { // B
    // yellow plastic
    GLfloat yellow_ambient[]  = {0.2f, 0.2f, 0.0f, 1.0f};
    GLfloat yellow_diffuse[]  = {0.7f, 0.7f, 0.0f, 1.0f};
    GLfloat yellow_specular[] = {0.8f, 0.8f, 0.8f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   yellow_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   yellow_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  yellow_specular);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 70.4f);
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
    if(j==N-1) { // F
    // white plastic
    GLfloat white_ambient[]  = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat white_diffuse[]  = {0.7f, 0.7f, 0.8f, 1.0f};
    GLfloat white_specular[] = {0.8f, 0.8f, 0.8f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   white_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   white_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  white_specular);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 70.4f);
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
    if(k==0) { // L
    // orange plastic
    GLfloat orange_ambient[]  = {0.2f, 0.0f, 0.0f, 1.0f};
    GLfloat orange_diffuse[]  = {0.8f, 0.5f, 0.0f, 1.0f};
    GLfloat orange_specular[] = {0.8f, 0.8f, 0.7f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   orange_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   orange_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  orange_specular);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 70.4f);
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
    if(k==N-1) { // R
    // red plastic
    GLfloat red_ambient[]  = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat red_diffuse[]  = {0.7f, 0.0f, 0.0f, 1.0f};
    GLfloat red_specular[] = {0.8f, 0.8f, 0.8f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   red_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   red_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  red_specular);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 70.4f);
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
    // blue plastic
    GLfloat blue_ambient[]  = {0.0f, 0.0f, 0.1f, 1.0f};
    GLfloat blue_diffuse[]  = {0.0f, 0.0f, 0.6f, 1.0f};
    GLfloat blue_specular[] = {0.8f, 0.8f, 0.8f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   blue_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   blue_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  blue_specular);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 70.4f);
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
    if(l==N-1) { // U
    // green plastic
    GLfloat green_ambient[]  = {0.0f, 0.1f, 0.0f, 1.0f};
    GLfloat green_diffuse[]  = {0.0f, 0.4f, 0.1f, 1.0f};
    GLfloat green_specular[] = {0.8f, 0.8f, 0.8f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   green_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   green_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  green_specular);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 70.4f);
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

/*****************************************
 * struct Cubic
 ----------------------------------------*/
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

/*****************************************
 * main Object
 ----------------------------------------*/
int N = 4; // Cubic-Size

#define MAXSIZE 8
Cubic cubic[ MAXSIZE * MAXSIZE * MAXSIZE ];

#define STACKSIZE 10000
Action  act[ STACKSIZE ], *asp = act;

/*****************************************
 * global parameters 
 ----------------------------------------*/
/* flags */
int zspin = 0;
int scramble  = 0;

/* stack tracing mode */
int trace_stack = 0; // 0: not in the trace mode, 1: foreward, -1: backward;

/* rotation step */
GLfloat dangle = 90.0/64;

/* zspin rot angle */
GLfloat zangle = 0;
#define dzangle (dangle/18)

/* data file name */
char *DataFile = NULL;

/*****************************************
 * setup Cubic
 ----------------------------------------*/
void initCubic(void)
{
    Matrix E = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    int i;
    for (i = 0; i < N*N*N; i++) {
        memcpy(cubic[i].M, E, sizeof(Matrix)); 
        cubic[i].M[12] = (i%N)       - .5*(N-1);
        cubic[i].M[13] = (i%(N*N))/N - .5*(N-1);
        cubic[i].M[14] = (i/(N*N))   - .5*(N-1);
    }
    asp = act;
}

void printCube(void)
{
    int i;
    for (i = 0; i < N*N*N; i++) {
        //printf("%f,%f,%f\n",   cubic[i].M[0], cubic[i].M[4], cubic[i].M[8]);
        //printf("%f,%f,%f\n",   cubic[i].M[1], cubic[i].M[5], cubic[i].M[9]);
        //printf("%f,%f,%f\n\n", cubic[i].M[2], cubic[i].M[6], cubic[i].M[10]);

        int j=(i%N), k=(i%(N*N))/N, l=(i/(N*N));

        if(j==0) { // -X
            // yellow plastic
            //printf("%f,%f,%f\n", cubic[i].M[0], cubic[i].M[4], cubic[i].M[8]);
            //printf("%f,%f,%f\n", cubic[i].M[1], cubic[i].M[5], cubic[i].M[9]);
            //printf("%f,%f,%f\n", cubic[i].M[2], cubic[i].M[6], cubic[i].M[10]);
            printf("%f,%f,%f\n", cubic[i].M[12], cubic[i].M[13], cubic[i].M[14]);
            printf("\n");
        }
        if(j==N-1) { //  X
            // green plastic
            //printf("%f,%f,%f\n", cubic[i].M[12], cubic[i].M[13], cubic[i].M[14]);
        }
        if(k==0) { // -Y
            // orange plastic
            //printf("%f,%f,%f\n", cubic[i].M[12], cubic[i].M[13], cubic[i].M[14]);
        }
        if(k==N-1) { //  Y 
            // red plastic
            //printf("%f,%f,%f\n", cubic[i].M[12], cubic[i].M[13], cubic[i].M[14]);
        }
        if(l==0) { // -Z
            // blue plastic
            //printf("%f,%f,%f\n", cubic[i].M[12], cubic[i].M[13], cubic[i].M[14]);
        }
        if(l==N-1) { // Z
            // white plastic
            //printf("%f,%f,%f\n", cubic[i].M[12], cubic[i].M[13], cubic[i].M[14]);
        }
    }
}

/*****************************************
 * Cubic action
 ----------------------------------------*/
void MultMatrix(Matrix M, Matrix A, Matrix K)   /* M = AK */
{
    int i, j;
    Matrix n; memcpy(n, K, sizeof(Matrix));
    for (i = 0; i < 3; i++) for (j = 0; j < 4; j++)
	M[i+4*j] = A[i+4*0]*n[0+4*j] + A[i+4*1]*n[1+4*j]
	    + A[i+4*2]*n[2+4*j] + A[i+4*3]*n[3+4*j];
}

void draw_cube(int i)
{
    int N1 = N-1;
    int j = (i%N), k = (i%(N*N))/N, l = (i/(N*N));
    if (j==0 || j-N1==0 || k==0 || k-N1==0 || l==0 || l-N1==0) {
	glPushMatrix();
	glMultMatrixf(cubic[i].M);
	draw_raw_cube(N);
	put_stickers(i, N);
	glPopMatrix();
    }
}

void drawCubic(void)
{
    int i = N*N*N;
    while (--i >= 0) draw_cube(i);   
}

void rotSlices(Action *a, int k)
{
    static Vector Ax_3[7] = {{ 0, 0,-1}, { 0,-1, 0}, {-1, 0, 0}, { 0, 0, 0},
			     { 1, 0, 0}, { 0, 1, 0}, { 0, 0, 1}};
    static Vector *Ax = Ax_3 + 3;
    static Matrix R90_3[7] = {
	{ 0,-1, 0, 0,  1, 0, 0, 0,  0, 0, 1, 0,  0, 0, 0, 1},   /* -z */
	{ 0, 0, 1, 0,  0, 1, 0, 0, -1, 0, 0, 0,  0, 0, 0, 1},   /* -y */
	{ 1, 0, 0, 0,  0, 0,-1, 0,  0, 1, 0, 0,  0, 0, 0, 1},   /* -x */
	{ 1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  0, 0, 0, 1},   /*  0 */
	{ 1, 0, 0, 0,  0, 0, 1, 0,  0,-1, 0, 0,  0, 0, 0, 1},   /* +x */
	{ 0, 0,-1, 0,  0, 1, 0, 0,  1, 0, 0, 0,  0, 0, 0, 1},   /* +y */
	{ 0, 1, 0, 0, -1, 0, 0, 0,  0, 0, 1, 0,  0, 0, 0, 1},   /* +z */
    };
    static Matrix *R90 = R90_3 + 3;
    GLubyte slice[8*8*8];
    GLfloat angle;
    int i;

    if (a->mask == 0) return;
    for (i = 0; i < N*N*N; i++) {
	slice[i] = (GLubyte)(cubic[i].M[11+abs(a->axis)] + .5*(N-1));
    }
    for (angle = k*dangle; -90 < angle && angle < 90; angle += k*dangle) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	if (zspin) glRotatef(zangle+=dzangle/2, 0., 0.,-1.);
        for (i = 0; i < N*N*N; i++) {
            glPushMatrix();
            if (a->mask & (1 << slice[i]))
                glRotatef(angle, Ax[a->axis].x, Ax[a->axis].y, Ax[a->axis].z);
	    draw_cube(i);
            glPopMatrix();
	}
	glPopMatrix();
	glutSwapBuffers();
    }
    for (i = 0; i < N*N*N; i++) {
	if (a->mask & (1 << slice[i])) {
	    int sign = (k>0) ? 1: -1;
	    MultMatrix(cubic[i].M, R90[sign*a->axis], cubic[i].M);
	}
    }
    glutPostRedisplay();
}

void rotRandam(void)
{
    static GLubyte mask[20] = { 1, 2,  4, 3, 6,  8, 12, 14,  16, 24, 28,
				32, 48,  7,  64, 96, 56,  128, 192, 112};
    static GLbyte  axis[6]  = {1, -1, 2, -2, 3, -3};
    static int init = 0;
    if (!init) { srand((unsigned) time(NULL)); init=1; }
    if (asp < &act[STACKSIZE-1]) {
	asp->mask = mask[rand()%(2+3*(N-2))];
	asp->axis = axis[rand()%6];
	rotSlices(asp++, 10);
    }
}

/**********************************************************************
 *  putString()
 ---------------------------------------------------------------------*/
char *message = NULL;

GLfloat MessageColor[4] = {0.,1.,1.,1.};

void setMassageColor(GLfloat r, GLfloat g, GLfloat b)
{
    MessageColor[0]=r; MessageColor[1]=g; MessageColor[2]=b;
}

void putString(int x, int y, char *s)
{
    int width  = glutGet((GLenum) GLUT_WINDOW_WIDTH);
    int height = glutGet((GLenum) GLUT_WINDOW_HEIGHT);
    glDisable(GL_LIGHTING);
    glColor4fv(MessageColor);
    glMatrixMode(GL_PROJECTION); glPushMatrix();
    glLoadIdentity(); gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);  glPushMatrix();
    glLoadIdentity(); glRasterPos2f(x, y = (y>0)? y: height-18+y);
    for( ; *s; s++) {
	if (*s == '\n') glRasterPos2f(x, y-=21);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *s);
    }
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);  glPopMatrix();
    glEnable(GL_LIGHTING);
}

/*****************************************
 * camera (eye-point)
 ----------------------------------------*/
/* camera position = (dist, theta, phi) in spherical coord */
typedef struct {
    GLdouble dist, theta, phi;
    GLdouble fovy; 
} Camera;

Camera *eye;

void initCamera(void)
{
    static GLfloat dfactor[9] = {.0,.0,.67,1.0,1.13,1.33,1.57,1.81,2.05};
    static Camera default_eye = {30,68,30,13};
    eye = &default_eye;
    eye->dist = 30*dfactor[N];
}

/*****************************************
 * lighting
 ----------------------------------------*/
/* light0 */
GLfloat light0_ambient [] = { 0.2, 0.2, 0.2, 1.0};
GLfloat light0_diffuse [] = { 0.8, 0.8, 0.8, 1.0};
GLfloat light0_specular[] = { 1.0, 1.0, 1.0, 1.0};
GLfloat light0_position[] = {10.0,-7.0,10.0, 0.0};
/* light1 */
GLfloat light1_ambient [] = { 0.1, 0.1, 0.1, 1.0};
GLfloat light1_diffuse [] = { 0.5, 0.5, 0.5, 1.0};
GLfloat light1_specular[] = { 0.6, 0.6, 0.6, 1.0};
GLfloat light1_position[] = { 5.0, 6.0, 8.0, 0.0};
/* light2 */
GLfloat light2_ambient [] = { 0.1, 0.1, 0.1, 1.0};
GLfloat light2_diffuse [] = { 0.5, 0.5, 0.5, 1.0};
GLfloat light2_specular[] = { 0.6, 0.6, 0.6, 1.0};
GLfloat light2_position[] = { 0.0, 4.0,-2.0, 0.0};

void initLights(void)
{
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
    glLightfv(GL_LIGHT1, GL_AMBIENT,  light1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
    glLightfv(GL_LIGHT2, GL_AMBIENT,  light2_ambient);
    glLightfv(GL_LIGHT2, GL_DIFFUSE,  light2_diffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, light2_specular);
    glEnable(GL_LIGHTING);
}

/*****************************************
 * glut CallBack Functions:
 ----------------------------------------*/
void draw(void)
{
    /* set camera */
    const GLdouble Deg = M_PI/180;
    GLdouble r = eye->dist, Theta = eye->theta*Deg, Phi = eye->phi*Deg;
    GLdouble w = (GLdouble)glutGet(GLUT_WINDOW_WIDTH);
    GLdouble h = (GLdouble)glutGet(GLUT_WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(eye->fovy, w/h, 0.85*r, 1.2*r);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(r*sin(Theta)*cos(Phi), r*sin(Theta)*sin(Phi), r*cos(Theta),
	      0.001, 0.0, 0.0,
	      -cos(Theta)*cos(Phi), -cos(Theta)*sin(Phi), sin(Theta));

    /* put light */
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
    glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    
    /* draw cubic */
    glPushMatrix();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (zspin) glRotatef(zangle += dzangle, 0, 0,-1);
    drawCubic();
    glPopMatrix();

    /* show message */
    if (message) {
        putString(10,-10, message);
	free(message);
	message=NULL;
    }

    glutSwapBuffers();
}

void idle(void)
{
    static FILE *fp = NULL;

    if (DataFile || fp)
    {
	void makeAction(int);
	int c;
	if (fp == NULL && (fp = fopen(DataFile, "r")) == NULL) {
	    char mes[256];
	    sprintf(mes,"\"%s\" is not available", DataFile);
	    message = strdup(mes);
	    glutPostRedisplay();
	    DataFile = NULL;
	    return;
	}
	if (DataFile && (c=fgetc(fp)) != EOF) {
	    if (c == '#') while (getc(fp) != '\n');     /* skip comment */
	    makeAction(c);
	} else { // DataFile == NULL || fget(fp) == EOF ;
	    if (fp) { fclose(fp); fp = NULL; }
	    DataFile = NULL;
	    scramble = trace_stack = 0;
	}
    }
    else if (scramble)
    {
	rotRandam();
	scramble--;
	trace_stack = 0;
    }
    else if (trace_stack ==  1) /* trace forward */
    {
	if (asp->axis != 0) rotSlices(asp++, 1);
	else trace_stack = 0;
    }
    else if (trace_stack == -1) /* trace backward */
    {
	if (asp > act) rotSlices(--asp,-1);
	else trace_stack = 0;
    }

    if (zspin) glutPostRedisplay();
}

void onTimer(int value)
{
    idle();
    glutTimerFunc(value, onTimer, value);
}

typedef struct {
    GLdouble x, y, z;
} Vector3d;

Action getAction(Vector3d *pt0, Vector3d *mv)
{
    GLubyte mask[8]  = {1, 2, 4, 8, 16, 32, 64,128};
    GLdouble face_dist = .5*N;
    Action a = {0,0};
    if (fabs(fabs(pt0->x) - face_dist) < .03)
    {
	if (fabs(mv->y) > fabs(mv->z)) {
	    a.axis = (pt0->x*mv->y > 0) ? 3 : -3;
	    a.mask = mask[(int)(pt0->z + face_dist)];
	} else {
	    a.axis = (pt0->x*mv->z < 0) ? 2 : -2;
	    a.mask = mask[(int)(pt0->y + face_dist)];
	}
    }
    else if (fabs(fabs(pt0->y) - face_dist) < .03)
    {
	if (fabs(mv->z) > fabs(mv->x)) {
	    a.axis = (pt0->y*mv->z > 0) ? 1 : -1;
	    a.mask = mask[(int)(pt0->x + face_dist)];
	} else {
	    a.axis = (pt0->y*mv->x < 0) ? 3 : -3;
	    a.mask = mask[(int)(pt0->z + face_dist)];
	}
    }
    else if (fabs(fabs(pt0->z) - face_dist) < .03)
    {
	if (fabs(mv->x) > fabs(mv->y)) {
	    a.axis = (pt0->z*mv->x > 0) ? 2: -2;
	    a.mask = mask[(int)(pt0->y + face_dist)];
	} else {
	    a.axis = (pt0->z*mv->y < 0) ? 1 : -1;
	    a.mask = mask[(int)(pt0->x + face_dist)];
	}
    }
    return a;
}

#define GLUT_MOVE  2    /* GLUT_DOWN = 0, GLUT_UP = 1 */

void mouse(int button, int state, int x, int y)
{
    static GLdouble mvmatrix[16], projmatrix[16];
    static GLint viewport[4];
    static Vector3d pt, pt0, mv;
    static int selecting = 0;
    GLfloat z; /* depth value */

    if (DataFile || trace_stack || zspin) return;
    if (button != GLUT_LEFT_BUTTON) return;
    switch (state) {
    case GLUT_DOWN:
        glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
        glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);
        glGetIntegerv(GL_VIEWPORT, viewport);
        y = viewport[3] - 1 - y;  /* mouse pos to raster pos */
        glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
        if (z == 1.0) {
            static const GLdouble Deg=M_PI/180;
            GLdouble theta = (atan2(y-viewport[3]/2,x-viewport[2]/2))/Deg;
            asp->mask = 255;
            asp->axis =
                (theta<-140.)? -3:
                (theta< -80.)?  2:
                (theta< -30.)? -1:
                (theta<  40.)?  3:
                (theta< 100.)? -2:
                (theta< 150.)?  1:
                /*theta>150 */ -3;
            rotSlices(asp++, 1);
            return;
        }
	gluUnProject(x, y, z, mvmatrix, projmatrix, viewport,
		     &pt.x, &pt.y, &pt.z);
        pt0 = pt;
        selecting = 1;
        return;
    case GLUT_MOVE:
        if (selecting == 0) return;
        y = viewport[3] - 1 - y;  /* mouse pos to raster pos */
        glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
	gluUnProject(x, y, z, mvmatrix, projmatrix, viewport,
		     &pt.x, &pt.y, &pt.z);
	mv.x=pt.x-pt0.x, mv.y=pt.y-pt0.y, mv.z=pt.z-pt0.z;
	if (mv.x*mv.x+mv.y*mv.y+mv.z*mv.z < .1) return;
	*asp = getAction(&pt0, &mv);
	rotSlices(asp++, 1);
        selecting = 0;
        return;
    case GLUT_UP:
        selecting = 0;
        return;
    default:
        return;
    }
}

void motion(int x, int y) { mouse(GLUT_LEFT_BUTTON, GLUT_MOVE, x, y); }

void makeAction(int c)
{
    void writeLog(char *);
    static Action a;
    static int selecting = 0;
    static GLfloat bgg = 0.6; // background gray-level;
    const GLdouble root2 = sqrt(2.0);

    switch (c) {
    case 'q':  writeLog("cubic.log"); exit(0);
    case '0':  a.mask=  0; return;
    case ' ':  a.mask=255; return;
    case '1':  if(selecting++) a.mask|=1<<0; else a.mask=1<<0; return;
    case '2':  if(selecting++) a.mask|=1<<1; else a.mask=1<<1; return;
    case '3':  if(selecting++) a.mask|=1<<2; else a.mask=1<<2; return;
    case '4':  if(selecting++) a.mask|=1<<3; else a.mask=1<<3; return;
    case '5':  if(selecting++) a.mask|=1<<4; else a.mask=1<<4; return;
    case '6':  if(selecting++) a.mask|=1<<5; else a.mask=1<<5; return;
    case '7':  if(selecting++) a.mask|=1<<6; else a.mask=1<<6; return;
    case '8':  if(selecting++) a.mask|=1<<7; else a.mask=1<<7; return;
    case 'j':  a.axis= 1; *asp=a; rotSlices(asp++,1); break;
    case 'k':  a.axis= 2; *asp=a; rotSlices(asp++,1); break;
    case 'l':  a.axis= 3; *asp=a; rotSlices(asp++,1); break;
    case 'u':  a.axis=-1; *asp=a; rotSlices(asp++,1); break;
    case 'i':  a.axis=-2; *asp=a; rotSlices(asp++,1); break;
    case 'o':  a.axis=-3; *asp=a; rotSlices(asp++,1); break;
    case 'J':  a.axis=-1; *asp=a; rotSlices(asp++,1); break;
    case 'K':  a.axis=-2; *asp=a; rotSlices(asp++,1); break;
    case 'L':  a.axis=-3; *asp=a; rotSlices(asp++,1); break;
    case 'r':  DataFile=NULL; trace_stack=scramble=0; initCubic(); break;
    case 'R':  trace_stack=scramble=0; initCubic(); break;
    case 's':  scramble = 30; return;
    case 'S':  scramble = (scramble)? 0:-1; return;
    case 'b':  if(asp > act) rotSlices(--asp,-1); break;
    case 'f':  if(asp->axis != 0) rotSlices(asp++,1); break;
    case 'B':  trace_stack=(trace_stack)? 0:-1; return;
    case 'F':  trace_stack=(trace_stack)? 0: 1; return;
    case 'D':  eye->dist *=.9; break;
    case 'd':  eye->dist /=.9; break;
    case 'a':  zspin = !zspin; zangle = 0; break;
    case 'c':  zspin = !zspin; zangle = 0; break;
    case 'p':  printCube(); break;
    case '*':  dangle *= root2; return;
    case ':':  dangle *= root2; return;
    case '/':  dangle /= root2; return;
    case 'g':  if((bgg+=.05)>1) bgg=1; glClearColor(bgg,bgg,bgg,1); break;
    case 'G':  if((bgg-=.05)<0) bgg=0; glClearColor(bgg,bgg,bgg,1); break;
    case 1002: dangle = 90.0/  4; break;
    case 1003: dangle = 90.0/  8; break;
    case 1004: dangle = 90.0/ 16; break;
    case 1005: dangle = 90.0/ 32; break;
    case 1006: dangle = 90.0/ 64; break;
    case 1007: dangle = 90.0/128; break;
    case 1008: dangle = 90.0/256; break;
    case 3002: N=2; initCamera(); trace_stack=scramble=0; initCubic(); break;
    case 3003: N=3; initCamera(); trace_stack=scramble=0; initCubic(); break;
    case 3004: N=4; initCamera(); trace_stack=scramble=0; initCubic(); break;
    case 3005: N=5; initCamera(); trace_stack=scramble=0; initCubic(); break;
    case 3006: N=6; initCamera(); trace_stack=scramble=0; initCubic(); break;
    case 3007: N=7; initCamera(); trace_stack=scramble=0; initCubic(); break;
    case 3008: N=8; initCamera(); trace_stack=scramble=0; initCubic(); break;
    case  'N': N=8; {
	while(!(a.mask & 1<<--N) && N > 1); N++;
	initCamera(); trace_stack=scramble=0; initCubic(); break;
    }
    default:   return;
    }
    selecting=0;
    glutPostRedisplay();
}

void key(unsigned char c, int x, int y) {  makeAction((int) c); }

void skey(int k, int x, int y)
{
    switch (k) {
    case GLUT_KEY_UP:    eye->theta -= 2; break;
    case GLUT_KEY_DOWN:  eye->theta += 2; break;
    case GLUT_KEY_LEFT:  eye->phi   -= 2; break;
    case GLUT_KEY_RIGHT: eye->phi   += 2; break;
    default: return;
    }
    glutPostRedisplay();
}

void makeMenu(void)
{
    GLint rot_step, cube_size;
    cube_size = glutCreateMenu(makeAction); {
        glutAddMenuEntry(" 2",            3002);
        glutAddMenuEntry(" 3",            3003);
        glutAddMenuEntry(" 4",            3004);
        glutAddMenuEntry(" 5",            3005);
        glutAddMenuEntry(" 6",            3006);
        glutAddMenuEntry(" 7",            3007);
        glutAddMenuEntry(" 8",            3008);
    }
    rot_step = glutCreateMenu(makeAction); {
        glutAddMenuEntry("  8",           1003);
        glutAddMenuEntry(" 16",           1004);
        glutAddMenuEntry(" 32",           1005);
        glutAddMenuEntry(" 64",           1006);
        glutAddMenuEntry("128",           1007);
        glutAddMenuEntry("256",           1008);
    }
    glutCreateMenu(makeAction); {
        glutAddMenuEntry("Undo",             'b');
        glutAddMenuEntry("Redo",             'f');
        glutAddMenuEntry("Scramble",         's');
        glutAddMenuEntry("Reset Cube",       'r');
        glutAddMenuEntry("Trace Back",       'B');
        glutAddMenuEntry("Toggle Z-Spin",    'a');
        glutAddMenuEntry("Close up",         'D');
        glutAddMenuEntry("Move away",        'd');
        glutAddSubMenu("Cubic Size",   cube_size);
        glutAddSubMenu("Rotation Step", rot_step);
        glutAddMenuEntry("Quit",             'q');
    }
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void reshape(int width, int height) { glViewport(0, 0, width, height); }

/*****************************************
 * main
 ----------------------------------------*/
int main(int argc, char *argv[])
{
    void usage(void);
    int idx;

    for (idx = 1; idx < argc; idx++)
	if (*argv[idx] == '-') {
            if (strcmp(argv[idx], "-n") == 0) {
		if (argc <= ++idx || sscanf(argv[idx], "%d", &N) != 1) {
		    fputs("missing size\n",stderr); usage(); exit(1);
		} else if (N > 8) {
		    fputs("Size must be <= 8.\n\n",stderr); exit(1);
		}
	        continue;
	    }
	    if (strcmp(argv[idx], "-s") == 0) {
		int exp; /* default: 4 */
		if (argc <= ++idx || sscanf(argv[idx], "%d", &exp) != 1) {
		    fputs("missing step number\n",stderr); usage(); exit(1);
		}
		dangle = 90.0/(1<<exp);
		continue;
	    }
	    if (strcmp(argv[idx], "-bg") == 0) {
		GLfloat bgg;
		if (argc <= ++idx || sscanf(argv[idx],"%f", &bgg) != 1) {
		    fputs("missing float\n",stderr); usage(); exit(1);
		}
	        continue;
	    } else {
		fprintf(stderr, "illegal option '%s'\n", argv[idx]);
		usage(); exit(1);
	    }
	} else { /* Should be an input file */
	    DataFile = strdup(argv[idx]);
	}

    /* initialize */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(640, 500);
    glutCreateWindow(argv[0]);
    glClearColor(0.6,0.6,0.6,1.0);

    /* setup Cubes */
    initCubic();
    initCamera();
    initLights();

    /* shading-modes */
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);

    /* regist call-back functions */
    glutDisplayFunc(draw);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(key);
    glutSpecialFunc(skey);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutTimerFunc(30, onTimer, 30);
    //glutIdleFunc(idle);
    makeMenu();
    
    glutMainLoop();
    return 0;
}

/*****************************************
 * usage
 ----------------------------------------*/
void usage(void)
{
    fprintf(stderr, 
	    "\n"
	    "cubic [-n N] [-s n] [file]\n"
	    "\n"
	    "-n N : size = N*N*N\n"
	    "-s n : rot_step = 2^n\n"
	    "file : read action from the file\n"
	    "\n"
	    "Keys:\n"
	    "   1, 2, 3 : select 'slice'\n"
	    "   space   : all\n"
	    "   j, k, l : rotate cubes\n"
            "   u, i, o : j,u = x; k,i = y; l,o = z\n"
	    "   s       : scramble\n"
	    "   b       : back 1 step (Undo)\n"
	    "   f       : procede 1 step (Redo)\n"
	    "   a       : toggle zspin\n"
	    "\n");
}

/*****************************************
 * write log
 ----------------------------------------*/
void writeLog(char *fname)
{
    char *c_3[7] = {"o", "i", "u", "", "j", "k", "l"}, **c = c_3 + 3;
    Action *ap;
    FILE *fp;
    int i;

    if ((fp = fopen(fname, "w")) == NULL) return;
    fprintf(fp, "%dN\n#\n", N);
    for (ap = act; ap < asp; ap++) {
	if (ap->mask == 0) ;
	else if (ap > act && ap->mask == (ap-1)->mask) {
	    /* && ap->axis == (ap-1)->axis) { */
	    fprintf(fp, "%s", c[ap->axis]);
	} else {
	    fputs(" ", fp);
	    for (i=1; i<=N; i++) {
		if (ap->mask & (1<<(i-1)))
		    fprintf(fp, "%d", i);
	    }
	    fprintf(fp, "%s", c[ap->axis]);
        }
    }
    fputs("\n#EOF\n", fp);
    fclose(fp);
}