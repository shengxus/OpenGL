#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <cstdlib>
#include "SOIL.h"
#include "OBJParser.h"

using namespace std;

GLuint texture;
OBJParser parser;

/* white ambient light at half intensity (rgba) */
GLfloat LightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };

/* super bright, full intensity diffuse light. */
GLfloat LightDiffuse[] = { 0.0f, 0.8f, 0.0f, 1.0f };

/* position of light (x, y, z, (position of light)) */
GLfloat LightPosition[] = { 0.0f, 0.0f, 2.0f, 1.0f };

void init(){
	// clean the screen
	glClearColor(0,0,0,0);
	// choose the shader type: GL_SMOOTH or GL_FLAT
	glShadeModel(GL_SMOOTH);
	// best perspective correction
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_DEPTH_TEST);
	// z-buffer type
	glDepthFunc(GL_LEQUAL);
	parser.parse("test.obj");
	
	// set up light number 1.
    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);  // add lighting. (ambient)
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);  // add lighting. (diffuse).
    glLightfv(GL_LIGHT1, GL_POSITION,LightPosition); // set light position.
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);                             // turn light 1 on.
}

GLfloat theta = 0.0f;

void display(){
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	// change the transform matrices in GL_MODELVIEW stack (only have an effect on individual object)
	// pay attention to the order.
	glLoadIdentity();
	// change to the camera space
	gluLookAt(0,50,0,0,50,-100,0,1,0);
	// change to the world space
	glTranslatef(0.0f, -30.0f, -100.0f);
	glRotatef(theta++, 0.0, 1.0, 0.0);
	glRotatef(90,0,0,1);
	glRotatef(90,0,1,0);
	// describe the model
	int num = parser.getNumOfFaces();
	int i=1;
	for(int i=1; i <= num; i++){
		Face f = parser.getFace(i);
		glBegin(GL_TRIANGLES);		 // begin drawing a triangle
		for(int y=0; y < f.faceVertices.size(); y++) {
			Face::FaceVertex fVertex = f.faceVertices[y];
			VertexNormal n = parser.getVertexNormal(fVertex.vn);
			Vertex v = parser.getVertex(fVertex.v);
			glNormal3f( n.x, n.y, n.z);                             
			glVertex3f( v.x, v.y, v.z);
		}
			glEnd();
	}
	// necessary if display mode is GLUT_DOUBLE
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(120,(GLfloat)w/(GLfloat)h,0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void processNormalKeys(unsigned char key,int x,int y)
{
	if(key==27)
		exit(0);
}


void idle(void){
	// necessary if display mode is GLUT_DOUBLE
	glutPostRedisplay();
}

int main(int argc, char *argv[]){
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
	glutInitWindowSize(500,500);
	glutInitWindowPosition(100,100);
	glutCreateWindow(0);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutKeyboardFunc(processNormalKeys);
	glutMainLoop();
    return 0;
}

