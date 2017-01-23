#include <GL/glew.h>
#include "axes.h"

#define BUFFER_OFFSET(x) ((const void*) (x))

//  Constructors
axes::axes()
{
}

//  Destructor
axes::~axes()
{
}

void axes::load(GLint v, GLint c)
{
    vPosition = v;
    vColor = c;

    GLushort indices[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

    GLfloat points[] = {0, 0, 0, 1,
                        1, 0, 0, 1,
                        0, 0, 0, 1,
                        -1, 0, 0, 1,
                        0, 0, 0, 1,
                        0, 1, 0, 1,
                        0, 0, 0, 1,
                        0, -1, 0, 1,
                        0, 0, 0, 1,
                        0, 0, 1, 1,
                        0, 0, 0, 1,
                        0, 0, -1, 1,
                       };

    GLfloat colors[] = {1, 0, 0,
                        1, 0, 0,
                        0.25, 0, 0,
                        0.25, 0, 0,
                        0, 1, 0,
                        0, 1, 0,
                        0, 0.25, 0,
                        0, 0.25, 0,
                        0, 0, 1,
                        0, 0, 1,
                        0, 0, 0.25,
                        0, 0, 0.25
                       };

    glGenVertexArrays(1, &vboptr);
    glBindVertexArray(vboptr);

    GLuint eboptr;
    glGenBuffers(1, &eboptr);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboptr);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &bufptr);
    glBindBuffer(GL_ARRAY_BUFFER, bufptr);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_DYNAMIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
    glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_TRUE, 0, BUFFER_OFFSET(sizeof(points)));
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    glEnableVertexAttribArray(vPosition);
    glEnableVertexAttribArray(vColor);
}

void axes::draw()
{
    glLineWidth(2);
    glBindVertexArray(vboptr);
    glDrawElements(GL_LINES, 12, GL_UNSIGNED_SHORT, NULL);
    glLineWidth(1);
}
