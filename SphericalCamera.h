#ifndef SPHERICALCAMERA_H
#define SPHERICALCAMERA_H

#include <GL/gl.h>
#include "vec_mat.h"
#include <math.h>

#define PI 3.14159265358979323846
#define PI_DIV_180 0.017453292519943296
#define deg PI_DIV_180

class SphericalCamera
{
public:
    SphericalCamera();
    void setPosition(float R, float Theta, float Psy);
    mat4 lookAt();

    void addR(float num);
    void addTheta(float num);
    void addPsy(float num);

    void setR(float num);
    void setTheta(float num);
    void setPsy(float num);

    float getR();
    float getTheta();
    float getPsy();

    vec3 getPosition();

private:
    float r;
    float theta;
    float psy;
};

#endif
