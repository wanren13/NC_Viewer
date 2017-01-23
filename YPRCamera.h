#ifndef YPRCAMERA_H
#define YPRCAMERA_H

#include <GL/gl.h>
#include "vec_mat.h"

class YPRCamera
{
public:
    YPRCamera();
    void PositionCamera(float X, float Y, float Z,
                        float vX, float vY, float vZ,
                        float upX, float upY, float upZ);
    void setPosition(float X, float Y, float Z);
    void setView(float X, float Y, float Z);
    void setUp(float X, float Y, float Z);
    void setUpViaY();
    vec3 getPosition();
    vec3 getView();
    vec3 getUpVector();
    mat4 lookAt();

    void addPitch(float num);
    void addYaw(float num);
    void addRoll(float num);

    void moveForward(float num);
    void moveRight(float num);
    void moveUp(float num);

private:
    vec3 Position;
    vec3 View;
    vec3 UpVector;
};

#endif
