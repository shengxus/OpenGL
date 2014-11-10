#ifndef OBJECT_H
#define OBJECT_H

#include <gl/glut.h>

#define TEX_NUM_MAX 5

class Object{
protected:
	GLuint texture[TEX_NUM_MAX];
	int  texNum;
public:
	Object(){texNum = 0;};
	virtual void render(){};
	void loadTexture(char*);
	void bindTexture(int index=0);
};
#endif