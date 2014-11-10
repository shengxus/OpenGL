#include "Camera.h"
#include <gl\glut.h>
#include <cstring>
#include <cmath>

using namespace std;

void Camera::initCamera(const GzCoord position, const GzCoord direction, const GzCoord worldup, float fovVal){
	memcpy(eye, position, sizeof(GzCoord));
	memcpy(lookat, direction, sizeof(GzCoord));
	memcpy(up, worldup, sizeof(GzCoord));
	memset(angle, 0, sizeof(GzCoord));
	FOV = fovVal;
}

void Camera::move(int x, int y, int z){
	step[X] -= x;
	step[Y] -= y;
	step[Z] -= z;
}

void Camera::turn(int axis, int degree){
	if(axis!=X||degree<0&&angle[axis]>-90||degree>0&&angle[axis]<90)
		angle[axis]+=degree;	
}

void Camera::setCamera() const{
	gluLookAt(eye[X], eye[Y], eye[Z], lookat[X], lookat[Y], lookat[Z],
		up[X], up[Y], up[Z]);
	glRotatef(angle[X], -1,0,0);
	glRotatef(angle[Y], 0,-1,0);
	glRotatef(angle[Z], 0,0,-1);
	glTranslatef(step[X], step[Y], step[Z]);
}

float Camera::getFOV() const{
	return FOV;
}