/*********************************************************
 * cube.c
 --------------------------------------------------------*/
/*--------------------------------------------------------
 * The following modelling data for each cubic-piece are 
 * taken from 
 *
 *   "rubik.c"  by mfvianna@centroin.com.br, 
 *
 * which can be found in the directory: ./reference
 --------------------------------------------------------*/

/*********************************************************
 * public function
 *
 * extern void draw_raw_cube(int N);
 * extern void put_stickers(int i, int N);
 *
 --------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <GL/glut.h>

GLfloat 
AA[9] ={0, 0, 0.480, 0.470, 0.465, 0.460, 0.455, 0.450, 0.450},
EE[9] ={0, 0, 0.460, 0.425, 0.415, 0.405, 0.400, 0.395, 0.390};

void draw_raw_cube(int N)
{
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
    GLfloat dd = (N<4)? 0.505 : 0.510;
    GLfloat ee = EE[N];
    GLfloat ff = ee - ((N==2)?.02:.04);
    int j=(i%N), k=(i%(N*N))/N, l=(i/(N*N));

    if(j==0) { // -X
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
    if(j==N-1) { //  X
    // green plastic
    GLfloat green_ambient[]  = {0.0f, 0.1f, 0.0f, 1.0f};
    GLfloat green_diffuse[]  = {0.0f, 0.4f, 0.1f, 1.0f};
    GLfloat green_specular[] = {0.8f, 0.8f, 0.8f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   green_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   green_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  green_specular);
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
    if(k==0) { // -Y
    // orange plastic
    GLfloat orange_ambient[]  = {0.2f, 0.0f, 0.0f, 1.0f};
    GLfloat orange_diffuse[]  = {1.0f, 0.1f, 0.0f, 1.0f};
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
    if(k==N-1) { //  Y 
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
    if (l==0) { // -Z
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
    if(l==N-1) { // Z
    // white plastic
    GLfloat white_ambient[]  = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat white_diffuse[]  = {0.7f, 0.7f, 0.8f, 1.0f};
    GLfloat white_specular[] = {0.8f, 0.8f, 0.8f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   white_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   white_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  white_specular);
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
/* -------------------------------------------------------- */
// End of cube.c 
