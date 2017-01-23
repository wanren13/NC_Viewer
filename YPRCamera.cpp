#include "YPRCamera.h"

//  Default constructor -- position is at the origin, the lookat vector is pointing
//    down the negative z-axis, up is the positive y, yaw, pitch and roll are all set
//    to 0.
YPRCamera::YPRCamera()
{
    Position.x = 0;
    Position.y = 0;
    Position.z = 0;
    View.x = 0;
    View.y = 0;
    View.z = -1;
    UpVector.x = 0;
    UpVector.y = 1;
    UpVector.z = 0;
}

//  Sets position, view and up vectors.
void YPRCamera::PositionCamera(float X, float Y, float Z, float vX, float vY, float vZ, float upX, float upY, float upZ)
{
    Position.x = X;
    Position.y = Y;
    Position.z = Z;
    View.x = vX;
    View.y = vY;
    View.z = vZ;
    UpVector.x = upX;
    UpVector.y = upY;
    UpVector.z = upZ;
    UpVector = normalize(UpVector);
    View = normalize(View);
}

//  Sets position
void YPRCamera::setPosition(float X, float Y, float Z)
{
    Position.x = X;
    Position.y = Y;
    Position.z = Z;
}

//  Sets view
void YPRCamera::setView(float X, float Y, float Z)
{
    View.x = X;
    View.y = Y;
    View.z = Z;
    View = normalize(View);
}

//  Sets up
void YPRCamera::setUp(float X, float Y, float Z)
{
    UpVector.x = X;
    UpVector.y = Y;
    UpVector.z = Z;
    UpVector = normalize(UpVector);
}

void YPRCamera::setUpViaY()
{
    vec3 Y(0,1,0);
    vec3 V = View;
    vec3 P = dot(Y, V)/(length(V)*length(V))*V;

/*
    std::cout << Y << std::endl;
    std::cout << V << std::endl;
    std::cout << P << std::endl;
    std::cout << Y-P << std::endl;
//*/
    UpVector = normalize(Y-P);
}

//  Gets Position
vec3 YPRCamera::getPosition()
{
    return Position;
}

//  Gets View
vec3 YPRCamera::getView()
{
    return View;
}

//  Gets Up
vec3 YPRCamera::getUpVector()
{
    return UpVector;
}

//  Add to the pitch degree rotation.
void YPRCamera::addPitch(float num)
{
    vec3 mn = cross(View, UpVector);
    mat3 rot = Rotate3(num, mn);

    View = rot*View;
    UpVector = rot*UpVector;

    UpVector = normalize(UpVector);
    View = normalize(View);
}

//  Add to the yaw degree rotation.
void YPRCamera::addYaw(float num)
{
    mat3 rot = Rotate3(num, UpVector);
    View = rot*View;

    View = normalize(View);
}

//  Add to the roll degree rotation.
void YPRCamera::addRoll(float num)
{
    mat3 rot = Rotate3(num, View);
    UpVector = rot*UpVector;

    UpVector = normalize(UpVector);
}

//  Invoke LookAt with position, view and up
mat4 YPRCamera::lookAt()
{
    return LookAt(Position.x, Position.y, Position.z,
                  Position.x + View.x, Position.y + View.y, Position.z + View.z,
                  UpVector.x, UpVector.y, UpVector.z);
}

//  Move the camera forward
void YPRCamera::moveForward(float num)
{
    float mod = length(View);
    if (mod < 0.000001f) return;

    Position += View/mod*num;
}

//  Move the camera right
void YPRCamera::moveRight(float num)
{
    vec3 mn = cross(View, UpVector);

    float mod = length(mn);
    if (mod < 0.000001f) return;

    Position += mn/mod*num;
}

//  Move the camera up
void YPRCamera::moveUp(float num)
{
    float mod = length(UpVector);
    if (mod < 0.000001f) return;

    Position += UpVector/mod*num;
}
