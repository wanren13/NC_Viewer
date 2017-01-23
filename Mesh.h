#ifndef MESH_H
#define MESH_H

#include "util.h"

class Mesh
{
public:
    Mesh();
    ~Mesh();

    bool isCreated();

    void load(const QVector<GLfloat>& xVec,
              const QVector<GLfloat>& yVec,
              const QVector<GLfloat>& zVec,
              int timeNum, GLfloat minX, GLfloat maxX, GLfloat minZ, GLfloat maxZ,
              const QVector<QVector<GLint> >& tris,
              const QVector<QVector<TwoV> >& adjPts,
              int numOfNs, int numOfTs);
    void reloadData();
    void perturbReloadData();
    void draw();

    void setY(const QVector<GLfloat>& yVec, int timeNum);
    void setMinY(GLfloat y);
    void setMaxY(GLfloat y);

    void setColorCube(GLboolean d);

    void setColor(GLfloat r, GLfloat g, GLfloat b);
    void setBorderColor(GLfloat r, GLfloat g, GLfloat b);

    void perturbNormalsRandom(float a);

    GLfloat getMinY() const;
    GLfloat getMaxY() const;
private:
    GLfloat* points;
    GLfloat* normals;
    GLuint*  indices;
    GLfloat* colors;
    GLfloat* tex;

    QVector<GLfloat> x;
    QVector<GLfloat> y;
    QVector<GLfloat> z;
    int time;
    GLfloat minimumX;
    GLfloat maxiumX;
    GLfloat minimumZ;
    GLfloat maxiumZ;

    QVector<QVector<GLint> > t;
    QVector<QVector<TwoV> >  p;

    GLfloat minY;
    GLfloat maxY;

    int numOfNodes;
    int numOfTriangles;

    bool created;

    void CrossProd(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat *cx, GLfloat *cy, GLfloat *cz);

    void createNormals();

    GLboolean reload;
    GLboolean perturb;

    GLuint vboptr;
    GLuint bufptr;
    GLuint eboptr;
};

#endif // MESH_H
