#include "incl\Object.h"
#include "incl\SOIL.h"

using namespace std;

void Object::loadTexture(char *file){
	texture[texNum++] = SOIL_load_OGL_texture(file,
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y);
}

void Object::bindTexture(int index){
	glBindTexture(GL_TEXTURE_2D, texture[index]);
}