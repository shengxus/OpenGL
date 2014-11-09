#ifndef CAMERA_H
#define CAMERA_H

#include "Global.h"
class Camera{
private:
	GzCoord eye;		/* position of image plane origin */
	GzCoord lookat;		/* position of look-at-point */
	GzCoord up;			/* world up-vector (almost screen up) */
	GzCoord angle;
	GzCoord step;
	float   FOV;		/* horizontal field of view */
public:
	void initCamera(const GzCoord, const GzCoord, const GzCoord, float);
	void move(int, int, int);
	void turn(int, int);
	void setCamera() const;
	float getFOV() const;
};

#endif