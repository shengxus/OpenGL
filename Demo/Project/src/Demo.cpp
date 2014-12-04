#include <cstdlib>
#include "incl\Sky.h"
#include "incl\Camera.h"
#include "incl\Terrain.h"
#include "incl\Timer.h"
using namespace std;

Camera camera;
Sky sky;
Terrain terrain;
Timer timer;

float cameraParam[9] = {
	0, 0, 0,
	0, 0, -0.1,
	0, 1, 0,
};

void init(){
	camera.initCamera(cameraParam, cameraParam+3, 60);
	// clean the screen
	glClearColor(0, 0, 0, 0);
	// choose the shader type: GL_SMOOTH or GL_FLAT
	glShadeModel(GL_SMOOTH);
	// best perspective correction
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
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
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	camera.setCamera();
	sky.render();
	glPushMatrix();
	glTranslatef(-((GLfloat)MAP_SIZE * 0.5f), -80.f, -((GLfloat)MAP_SIZE * 0.5f));	
	terrain.reset();
	terrain.tessellate(camera);
	terrain.render();
	glPopMatrix();
	//timer.showTime(camera.getScreenWidth(), camera.getScreenHeight());
	glutSwapBuffers();
}

void reshape(int w, int h) {
	if (h == 0)
		h = 1;
	if (w == 0)
		w = 1;
	camera.setScreenSize(w, h);
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(camera.getFOV(), (GLfloat)w / (GLfloat)h, NEAR_CLIP, FAR_CLIP);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void pressKey(int key, int x1, int y1) {

	switch (key) {
	case GLUT_KEY_LEFT:    camera.turn(Y, -1); break;
	case GLUT_KEY_RIGHT:   camera.turn(Y, 1);  break;
	case GLUT_KEY_UP:      camera.move(1);     break;
	case GLUT_KEY_DOWN:    camera.move(-1);    break;
	case GLUT_KEY_F1:      camera.turn(X, 1);  break;
	case GLUT_KEY_F2:      camera.turn(X, -1); break;
	}
}

void processNormalKeys(unsigned char key, int x, int y) {
	if (key == 27)
		exit(0);
}

void idle(void){
	glutPostRedisplay();
}

int main(int argc, char *argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(640, 360);
	glutCreateWindow("Team Shadows - Project");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutSpecialFunc(pressKey);
	glutKeyboardFunc(processNormalKeys);
	glutMainLoop();
	return 0;
}