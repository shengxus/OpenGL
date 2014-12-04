#include "gl\glut.h"
#include "incl\Timer.h"
#include <cstdio>

void setOrthographicProjection(int w, int h) {

	// switch to projection mode
	glMatrixMode(GL_PROJECTION);
	// save previous matrix which contains the 
	//settings for the perspective projection
	glPushMatrix();
	// reset matrix
	glLoadIdentity();
	// set a 2D orthographic projection
	gluOrtho2D(0, w, 0, h);
	// invert the y axis, down is positive
	glScalef(1, -1, 1);
	// mover the origin from the bottom left corner
	// to the upper left corner
	glTranslatef(0, -h, 0);
	glMatrixMode(GL_MODELVIEW);
}

void resetPerspectiveProjection() {
	// set the current matrix to GL_PROJECTION
	glMatrixMode(GL_PROJECTION);
	// restore previous settings
	glPopMatrix();
	// get back to GL_MODELVIEW matrix
	glMatrixMode(GL_MODELVIEW);
}

void renderBitmapString(float x, float y, void *font,char *string)
{
  
  char *c;
  // set position to start drawing fonts
  glRasterPos2f(x, y);
  // loop all the characters in the string
  for (c=string; *c != '\0'; c++) {
    glutBitmapCharacter(font, *c);
  }
}


void renderSpacedBitmapString(float x, float y,int spacing, void *font,char *string) {
  char *c;
  int x1=x;
  for (c=string; *c != '\0'; c++) {
	glRasterPos2f(x1,y);
    glutBitmapCharacter(font, *c);
	x1 = x1 + glutBitmapWidth(font,*c) + spacing;
  }
}


void renderVerticalBitmapString(float x, float y, int bitmapHeight, void *font,char *string)
{
  
  char *c;
  int i;
  for (c=string,i=0; *c != '\0'; i++,c++) {
	glRasterPos2f(x, y+bitmapHeight*i);
    glutBitmapCharacter(font, *c);
  }
}

void renderStrokeString(float x, float y, float z, void *font,char *string)
{
  
  char *c;
  glPushMatrix();
  glTranslatef(x, y, z);
  for (c=string; *c != '\0'; c++) {
    glutStrokeCharacter(font, *c);
  }
  glPopMatrix();
}

void renderBitmapString(
		float x, 
		float y, 
		float z, 
		void *font, 
		char *string) {  
  char *c;
  glRasterPos3f(x, y,z);
  for (c=string; *c != '\0'; c++) {
    glutBitmapCharacter(font, *c);
  }
}

void Timer::showTime(int w, int h) {
    float timer = glutGet(GLUT_ELAPSED_TIME);
    frame++;
    if (timer - timeBase > 1000) {
        sprintf(s,"FPS:%4.2f",frame*1000.0/(timer-timeBase));
        timeBase = timer;
        frame = 0;
    }
    glColor3f(0.0f,1.0f,1.0f);
    setOrthographicProjection(w, h);
    glPushMatrix();
    glLoadIdentity();
    renderBitmapString(30,15,(void *)GLUT_BITMAP_8_BY_13,s);
    glPopMatrix();
    resetPerspectiveProjection();
}
