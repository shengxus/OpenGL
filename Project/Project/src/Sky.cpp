#include "incl\Sky.h"
#include <cmath>

using namespace std;

inline void getVertex(GzVertex &vertex, int phi, int theta){
	// Calculate the vertex at phi, theta
	vertex.coord[X] = RADIOUS * sin(DTOR(phi)) * cos(DTOR(theta));
	vertex.coord[Y] = RADIOUS * sin(DTOR(phi)) * sin(DTOR(theta));
	vertex.coord[Z] = RADIOUS * cos(DTOR(phi));
	// Normalize the vector
	float mag = (float)sqrt(SQR(vertex.coord[X]) + SQR(vertex.coord[Y]) + SQR(vertex.coord[Z]));
	// Calculate the spherical texture coordinates
	vertex.texCoord[U] = cos(DTOR(theta))*phi/180 + 0.5f;
	vertex.texCoord[V] = sin(DTOR(theta))*phi/180 + 0.5f;
}

inline void fixSeam(GzVertex &v1, GzVertex &v2){
	if (v1.texCoord[U] - v2.texCoord[U] > 0.9)
		v2.texCoord[U] += 1;
	else if (v2.texCoord[U] - v1.texCoord[U] > 0.9)
		v1.texCoord[U] += 1;
	if (v1.texCoord[V] - v2.texCoord[V] > 0.8)
		v2.texCoord[V] += 1;
	else if (v2.texCoord[V] - v1.texCoord[V] > 0.8)
		v1.texCoord[V] += 1;
}

void Sky::generateDome(){
	// Generate the dome
	for (int phi = 0, n = 0; phi <= 90 - DELTA_PHI; phi += DELTA_PHI){
		for(int theta = 0; theta <= 360 - DELTA_THETA; theta += DELTA_THETA) {
			getVertex(vertices[n++], phi, theta);
			getVertex(vertices[n++], phi + DELTA_PHI, theta);
			getVertex(vertices[n++], phi, theta + DELTA_THETA);
			if (phi > -90 && phi < 90){
				getVertex(vertices[n++], phi + DELTA_PHI, theta + DELTA_THETA);
			}
		}
	}

	//// Fix the problem at the seam
	for (int i = 0; i < vNum - 3; i++){
		fixSeam(vertices[i], vertices[i + 1]);
		fixSeam(vertices[i], vertices[i + 2]);
		fixSeam(vertices[i + 1], vertices[i + 2]);
	}
}

void Sky::render(){
	glPushMatrix();
	glTranslatef(0,-20,0);
	glRotatef(270, 1.0f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i < vNum; i++){
		glTexCoord2f(vertices[i].texCoord[U], vertices[i].texCoord[V]);
		glVertex3f(vertices[i].coord[X], vertices[i].coord[Y], vertices[i].coord[Z]);
	}
	glEnd();
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D,0);
}