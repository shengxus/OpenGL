#ifndef GRASS_H
#define GRASS_H

#include "Object.h"
#include "Global.h"
#define DELTA_THETA 5
#define DELTA_PHI 5
#define RADIOUS 600

class Sky :public Object{
private:
	GzVertex vertices[129600/(DELTA_PHI*DELTA_THETA)]; //(360/DELTA_THETA)*(90/DELTA_PHI)*4
	const static int vNum = 129600 / (DELTA_PHI*DELTA_THETA);
public:
	void generateDome();
	virtual void render();
};
#endif