#ifndef CAMERA_H
#define CAMERA_H

#include "Global.h"
class Camera{
private:
    int w, h, angle;
	GzCoord eye;		/* position of camera */
	GzCoord delta;
    float   frustum[6][4];
	float   FOV;		/* horizontal field of view */
public:
	void initCamera(const GzCoord, const GzCoord, float);
	void move(int);
	void turn(int, int);
    void setScreenSize(int, int);
    int getScreenWidth() const {return w;};
    int getScreenHeight() const {return h;};
	void setCamera() const;
	float getFOV() const;
    void getPosition(GzCoord tmp) const;
    void calFrustum();
    bool isVisible(const GzCoord pt) const;
};

#endif