#include <cmath>
#include "incl/Camera.h"
#include <gl/glut.h>
#include <cstring>


using namespace std;

void Camera::setScreenSize(int width, int height) {
    w = width;
    h = height;
}

void Camera::initCamera(const GzCoord position, const GzCoord len, float fovVal){
	memcpy(eye, position, sizeof(GzCoord));
	memcpy(delta, len, sizeof(GzCoord));
	FOV = fovVal;
    w = h = angle = 0;
}

void Camera::move(int step){
    eye[X] += step*delta[X]*0.1;
    eye[Z] += step*delta[Z]*0.1;
}

void Camera::turn(int axis, int degree){
    if(axis == Y) {
        angle += degree;
        delta[X] = sin(DTOR(angle));
        delta[Z] = -cos(DTOR(angle));
    }
    else if (axis == X)
        delta[Y] +=0.01*degree;
}

void Camera::setCamera() const{
    gluLookAt(eye[X], eye[Y], eye[Z],
              eye[X]+delta[X], eye[Y]+delta[Y], eye[Z]+delta[Z],
              0.0f,1.0f,0.0f);
}

float Camera::getFOV() const{
	return FOV;
}

inline void NormalizePlane(float frustum[6][4], int side) {
    // Here we calculate the magnitude of the normal to the plane (point A B C)
    // Remember that (A, B, C) is that same thing as the normal's (X, Y, Z).
    // To calculate magnitude you use the equation:  magnitude = sqrt( x^2 + y^2 + z^2)
    float magnitude = (float)sqrt( frustum[side][A] * frustum[side][A] +
                                  frustum[side][B] * frustum[side][B] +
                                  frustum[side][C] * frustum[side][C] );
    
    // Then we divide the plane's values by it's magnitude.
    // This makes it easier to work with.
    frustum[side][A] /= magnitude;
    frustum[side][B] /= magnitude;
    frustum[side][C] /= magnitude;
    frustum[side][D] /= magnitude; 
}

void Camera::getPosition(GzCoord tmp) const {
    memcpy(tmp, eye, sizeof(GzCoord));
}

void Camera::calFrustum() {
    float   proj[16];								// This will hold our projection matrix
    float   modl[16];								// This will hold our modelview matrix
    float   clip[16];								// This will hold the clipping planes
    
    // glGetFloatv() is used to extract information about our OpenGL world.
    // Below, we pass in GL_PROJECTION_MATRIX to abstract our projection matrix.
    // It then stores the matrix into an array of [16].
    glGetFloatv( GL_PROJECTION_MATRIX, proj );
    
    // By passing in GL_MODELVIEW_MATRIX, we can abstract our model view matrix.
    // This also stores it in an array of [16].
    glGetFloatv( GL_MODELVIEW_MATRIX, modl );
    
    // Now that we have our modelview and projection matrix, if we combine these 2 matrices,
    // it will give us our clipping planes.  To combine 2 matrices, we multiply them.
    
    clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
    clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
    clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
    clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];
    
    clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
    clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
    clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
    clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];
    
    clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
    clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
    clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
    clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];
    
    clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
    clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
    clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
    clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];
    
    // Now we actually want to get the sides of the frustum.  To do this we take
    // the clipping planes we received above and extract the sides from them.
    
    // This will extract the RIGHT side of the frustum
    frustum[RIGHT][A] = clip[ 3] - clip[ 0];
    frustum[RIGHT][B] = clip[ 7] - clip[ 4];
    frustum[RIGHT][C] = clip[11] - clip[ 8];
    frustum[RIGHT][D] = clip[15] - clip[12];
    
    // Now that we have a normal (A,B,C) and a distance (D) to the plane,
    // we want to normalize that normal and distance.
    
    // Normalize the RIGHT side
    NormalizePlane(frustum, RIGHT);
    
    // This will extract the LEFT side of the frustum
    frustum[LEFT][A] = clip[ 3] + clip[ 0];
    frustum[LEFT][B] = clip[ 7] + clip[ 4];
    frustum[LEFT][C] = clip[11] + clip[ 8];
    frustum[LEFT][D] = clip[15] + clip[12];
    
    // Normalize the LEFT side
    NormalizePlane(frustum, LEFT);
    
    // This will extract the BOTTOM side of the frustum
    frustum[BOTTOM][A] = clip[ 3] + clip[ 1];
    frustum[BOTTOM][B] = clip[ 7] + clip[ 5];
    frustum[BOTTOM][C] = clip[11] + clip[ 9];
    frustum[BOTTOM][D] = clip[15] + clip[13];
    
    // Normalize the BOTTOM side
    NormalizePlane(frustum, BOTTOM);
    
    // This will extract the TOP side of the frustum
    frustum[TOP][A] = clip[ 3] - clip[ 1];
    frustum[TOP][B] = clip[ 7] - clip[ 5];
    frustum[TOP][C] = clip[11] - clip[ 9];
    frustum[TOP][D] = clip[15] - clip[13];
    
    // Normalize the TOP side
    NormalizePlane(frustum, TOP);
    
    // This will extract the BACK side of the frustum
    frustum[BACK][A] = clip[ 3] - clip[ 2];
    frustum[BACK][B] = clip[ 7] - clip[ 6];
    frustum[BACK][C] = clip[11] - clip[10];
    frustum[BACK][D] = clip[15] - clip[14];
    
    // Normalize the BACK side
    NormalizePlane(frustum, BACK);
    
    // This will extract the FRONT side of the frustum
    frustum[FRONT][A] = clip[ 3] + clip[ 2];
    frustum[FRONT][B] = clip[ 7] + clip[ 6];
    frustum[FRONT][C] = clip[11] + clip[10];
    frustum[FRONT][D] = clip[15] + clip[14];
    
    // Normalize the FRONT side
    NormalizePlane(frustum, FRONT);
}

bool Camera::isVisible(const GzCoord pt) const {
    // If you remember the plane equation (A*x + B*y + C*z + D = 0), then the rest
    // of this code should be quite obvious and easy to figure out yourself.
    // In case don't know the plane equation, it might be a good idea to look
    // at our Plane Collision tutorial at www.GameTutorials.com in OpenGL Tutorials.
    // I will briefly go over it here.  (A,B,C) is the (X,Y,Z) of the normal to the plane.
    // They are the same thing... but just called ABC because you don't want to say:
    // (x*x + y*y + z*z + d = 0).  That would be wrong, so they substitute them.
    // the (x, y, z) in the equation is the point that you are testing.  The D is
    // The distance the plane is from the origin.  The equation ends with "= 0" because
    // that is true when the point (x, y, z) is ON the plane.  When the point is NOT on
    // the plane, it is either a negative number (the point is behind the plane) or a
    // positive number (the point is in front of the plane).  We want to check if the point
    // is in front of the plane, so all we have to do is go through each point and make
    // sure the plane equation goes out to a positive number on each side of the frustum.
    // The result (be it positive or negative) is the distance the point is front the plane.
    
    // Go through all the sides of the frustum
    for(int i = 0; i < 6; i++ ) {
        // Calculate the plane equation and check if the point is behind a side of the frustum
        if(frustum[i][A] * pt[X] + frustum[i][B] * pt[Y] + frustum[i][C] * pt[Z] + frustum[i][D] <= 0) {
            // The point was behind a side, so it ISN'T in the frustum
            return false;
        }
    }
    
    // The point was inside of the frustum (In front of ALL the sides of the frustum)
    return true;
}