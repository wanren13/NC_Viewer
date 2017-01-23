#ifndef AXES_H_INCLUDED
#define AXES_H_INCLUDED

#include "vec_mat.h"

class axes
{
public:
    axes();
    ~axes();

    void load(GLint v, GLint c);
    void draw();

private:
    GLuint vboptr;
    GLuint bufptr;
    GLint vPosition = 0;
    GLint vColor = 1;
};

#endif // AXES_H_INCLUDED
