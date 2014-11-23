#include <cstdlib>
#include "incl\Sky.h"
#include "incl\Grass.h"
#include "incl\Camera.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h> 
#include <string>
#include "incl\glut.h"
#include "incl\maths.h"
#include "incl\Billboards.h"
#include "incl\tga.h"
#include "incl\fonts.h"

using namespace std;

Camera camera;
Sky sky;
Grass grass;

float cameraParam[9] = {
	0, 0, 0,
	0, 0, -0.1,
	0, 1, 0,
};

float angle=0.0,deltaAngle = 0.0,ratio;
float x=0.0f,y=0.75f,z=5.0f;
float lx=0.0f,ly=0.0f,lz=-1.0f;
int deltaMove = 0, bla=0,deltaUp=0;
	int debug=1,type = 0;
unsigned int texName[5];
int w,h;
char s[30];
float timer=0,timebase=0;
int frame=0;
int font=(int)GLUT_BITMAP_8_BY_13;


void changeSize(int w1, int h1)
	{
	w=w1;
	h=h1;

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;

	ratio = 1.0f * w / h;
	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	w=w1;
	h=h1;
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set the clipping volume
	gluPerspective(45,ratio,1,1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(x, y, z, 
		      x + lx,y + ly,z + lz,
			  0.0f,1.0f,0.0f);

	}


void initScene() {

	camera.initCamera(cameraParam, cameraParam+3, cameraParam+6, 60);
	// clean the screen
	glClearColor(0, 0, 0, 0);
	// choose the shader type: GL_SMOOTH or GL_FLAT
	glShadeModel(GL_SMOOTH);
	// best perspective correction
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_DEPTH_TEST);
	// z-buffer type
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_2D);
	// choose the texture interpolation type
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// repeat the pattern when outside the image
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Load Sky
	sky.loadTexture("data/Sky.png");
	sky.bindTexture();
	sky.generateDome();

	// Load Grass
	tgaInfo *image[5];
	std::string imgNames[5] = {"data/grass.tga", "data/grass2.tga","data/grassVariety.tga", "data/tree2.tga", "data/Sky.tga"};
	glEnable(GL_DEPTH_TEST);
	glGenTextures(5, texName);

	for(int i = 0; i < 4; i++){
		std::string str = imgNames[i].c_str();
		char *cstr = new char[str.length() + 1];
		strcpy(cstr, str.c_str());
	
		image[i] = tgaLoad(cstr);
		glBindTexture(GL_TEXTURE_2D,texName[i]);
		glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_WRAP_S,	GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_WRAP_T,	GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_MAG_FILTER	,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_MIN_FILTER	,GL_LINEAR);
	
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[i]->width, image[i]->height, 
					0, GL_RGBA, GL_UNSIGNED_BYTE, image[i]->imageData);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		delete [] cstr;
	}
	
}

void orientMe(float ang) {
	lx = sin(ang);
	lz = -cos(ang);
}

void moveMeFlat(int i) {
	x = x + i*(lx)*0.1;
	z = z + i*(lz)*0.1;
}

void Lookup(int i) {
	ly +=0.01*i;
}


void renderScene(void) {

	float right[3],up[3];

	if (deltaMove)
		moveMeFlat(deltaMove);
	if (deltaUp)
		Lookup(deltaUp);
	if (deltaAngle) {
		angle += deltaAngle;
		orientMe(angle);
	}
	float cam[3] = {x,y,z};

	glLoadIdentity();
	gluLookAt(x, y, z, 
		      x + lx,y + ly,z + lz,
			  0.0f,1.0f,0.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw ground
	glColor3ub(72.0f,75.0f,36.0f);
	glBegin(GL_QUADS);
		glVertex3f(-100.0f, 0.0f, -100.0f);
		glVertex3f(-100.0f, 0.0f,  100.0f);
		glVertex3f( 100.0f, 0.0f,  100.0f);
		glVertex3f( 100.0f, 0.0f, -100.0f);
	glEnd();

	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_ALPHA_TEST);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
    glAlphaFunc(GL_GREATER, 0);

	// Sky
	sky.bindTexture();
	sky.render();

	//Grass type 0
	glBindTexture(GL_TEXTURE_2D,texName[0]);
	grass.render();

	//Grass type 1
	glBindTexture(GL_TEXTURE_2D,texName[1]);
	grass.render();

	//Grass type 2
	glBindTexture(GL_TEXTURE_2D,texName[2]);
	grass.render();

	glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);

	frame++;
	timer=glutGet(GLUT_ELAPSED_TIME);
	if (timer - timebase > 1000) {
		sprintf(s,"FPS:%4.2f",frame*1000.0/(timer-timebase));
		timebase = timer;		
		frame = 0;
	}

	// Render font for FPS indicator
	glColor3f(0.0f,1.0f,1.0f);
	setOrthographicProjection(w,h);
	glPushMatrix();
	glLoadIdentity();
	renderBitmapString(30,15,(void *)font,s); 
	glPopMatrix();
	resetPerspectiveProjection();
	glutSwapBuffers();
}



void pressKey(int key, int x1, int y1) {

	switch (key) {
		case GLUT_KEY_LEFT : deltaAngle = -0.01f;break;
		case GLUT_KEY_RIGHT : deltaAngle = 0.01f;break;
		case GLUT_KEY_UP : deltaMove = 1;break;
		case GLUT_KEY_DOWN : deltaMove = -1;break;
		case GLUT_KEY_F1 : deltaUp = 1;break;
		case GLUT_KEY_F2 : deltaUp = -1;break;
	}
}

void releaseKey(int key, int x, int y) {

	switch (key) {
		case GLUT_KEY_LEFT : 
		case GLUT_KEY_RIGHT : deltaAngle = 0.0f;break;
		case GLUT_KEY_UP : 
		case GLUT_KEY_DOWN : deltaMove = 0;break;
		case GLUT_KEY_F1: 
		case GLUT_KEY_F2: deltaUp = 0;break;
	}
}

void processNormalKeys(unsigned char key, int x, int y) {

	if (key == 27) 
		exit(0);
}

int main(int argc, char *argv[]){

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(640,360);
	glutCreateWindow("CSCI 580 - Project");
	initScene();

	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(pressKey);
	glutSpecialUpFunc(releaseKey);

	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);

	printf("Instructions:\n");
	printf("Use the arrow keys to navigate, F1 to look up, and F2 to look down\n");

	glutMainLoop();

	return(0);
}

