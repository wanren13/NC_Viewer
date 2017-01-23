#ifndef UTIL_H
#define UTIL_H

#include <QHash>
#include <QString>
#include <QVector>
#include <GL/glew.h>

/**************************************************************************
 * Define
 * ***********************************************************************/

#define F1D QHash<QString, QVector<GLfloat> >
#define F2D QHash<QString, QVector<QVector<GLfloat> > >

#define I1D QHash<QString, QVector<GLint> >
#define I2D QHash<QString, QVector<QVector<GLint> > >

/**************************************************************************
 * Structures
 * ***********************************************************************/
/*
struct Triangle
{
    int v1;
    int v2;
    int v3;
};

struct FloatData
{
    GLfloat* ptr;
    int      size;
    GLfloat  min;
    GLfloat  max;
};
*/
struct TwoV
{
    GLint v1;
    GLint v2;
};

/**************************************************************************
 * Functions
 * ***********************************************************************/

#endif // UTIL_H
