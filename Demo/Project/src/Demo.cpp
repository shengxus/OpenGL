#include <cstdlib>
#include "incl\Sky.h"
#include "incl\Camera.h"
#include "incl\Terrain.h"
using namespace std;

Camera camera;
Sky sky;
Terrain terrain;

float cameraParam[9] = {
	0, 0, 0,
	0, 0, -0.1,
	0, 1, 0,
};

void init(){
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

	sky.loadTexture("data/Sky.png");
	sky.generateDome();

	terrain.init("data/height.raw");
	terrain.loadTexture("data/land.jpg");
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	camera.setCamera();
	sky.render();
	glPushMatrix();
	glTranslatef(-((GLfloat)MAP_SIZE * 0.5f), -80.f, -((GLfloat)MAP_SIZE * 0.5f));	
	terrain.reset();
	terrain.tessellate(camera);
	terrain.render();
	glPopMatrix();
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(camera.getFOV(), (GLfloat)w / (GLfloat)h, NEAR_CLIP, FAR_CLIP);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void processNormalKeys(unsigned char key, int x, int y)
{
	if (key == 27)
		exit(0);
	if (key == 'i')			// move forward
		camera.move(0, 0, -5);
	if (key == 'k')			// move backward
		camera.move(0, 0, 5);
	if (key == 'j')			// move left
		camera.move(-5, 0, 0);
	if (key == 'l')			// move right
		camera.move(5, 0, 0);
	if (key == 'w')			// raise head
		camera.turn(X, 1);
	if (key == 's')			// bow head
		camera.turn(X, -1);
	if (key == 'a')			// turn left
		camera.turn(Y, 1);
	if (key == 'd')			// turn right
		camera.turn(Y, -1);
}


void idle(void){
	glutPostRedisplay();
}

int main(int argc, char *argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Demo");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutKeyboardFunc(processNormalKeys);
	glutMainLoop();
	return 0;
}

