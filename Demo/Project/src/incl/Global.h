#ifndef GLOBAL_H
#define GLOBAL_H

#define RED   0               /* array indicies for color vector */
#define GREEN 1
#define BLUE  2

#define X     0               /* array indicies for position vector */
#define Y     1
#define Z     2

#define U     0               /* array indicies for texture coords */
#define V     1

#define NEAR_CLIP	0.1
#define FAR_CLIP	2000

#define PI 3.1415926535
#define DTOR(x) (x*PI/180.0)
#define SQR(x) (x*x)

#define BOUNDARY 500

typedef float GzCoord[3];
typedef float GzTexIndex[2];
typedef int GzGrid[3];

struct GzVertex{
	GzCoord coord;
	GzTexIndex texCoord;
};

#endif