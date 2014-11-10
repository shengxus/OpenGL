// encapsulate the implementation for grass render as one class
// add definition here
#ifndef GRASS_H
#define GRASS_H

#include "Object.h"
class Grass:public Object{
public:
	virtual void render();
};
#endif