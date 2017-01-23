#include "Mesh.h"
#include <math.h>
#include <GL/glew.h>
#include <cstdlib>
#include <ctime>
#include <iostream>

#define BUFFER_OFFSET(x) ((const void*) (x))

Mesh::Mesh():
    points(NULL), normals(NULL), indices(NULL), colors(NULL), tex(NULL),
    x(NULL), y(NULL), z(NULL), minY(0), maxY(0), numOfNodes(0), numOfTriangles(0),
    created(false), reload(GL_FALSE)
{
    srand(static_cast<unsigned int>(clock()));
}

Mesh::~Mesh()
{
    if (points)         delete [] points;
    if (normals)        delete [] normals;
    if (indices)        delete [] indices;
    if (colors)         delete [] colors;
    if (tex)            delete [] tex;
}

void Mesh::CrossProd(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat *cx, GLfloat *cy, GLfloat *cz)
{
    *cx = y1*z2-y2*z1;
    *cy = -x1*z2+z1*x2;
    *cz = x1*y2-x2*y1;
}

void Mesh::createNormals()
{
    GLfloat v1x, v1y, v1z, v2x, v2y, v2z;
    GLfloat cx, cy, cz;
    GLfloat scx, scy, scz;
    int v1, v2;
    GLfloat mod;

    for (int i = 0; i < numOfNodes; i++){
        scx = 0;
        scy = 0;
        scz = 0;
        for (int j = 0; j < p[i].size(); j++)
        {
            v1 = p[i][j].v1;
            v2 = p[i][j].v2;

            v1x = x[v1] - x[i];
            v1y = y[v1] - y[i];
            v1z = z[v1] - z[i];
            v2x = x[v2] - x[i];
            v2y = y[v2] - y[i];
            v2z = z[v2] - z[i];

            CrossProd(v1x, v1y, v1z, v2x, v2y, v2z, &cx, &cy, &cz);

            if(cy < 0){
                cx = -1.0 * cx;
                cy = -1.0 * cy;
                cz = -1.0 * cz;
            }

            mod = sqrt(cx*cx + cy*cy + cz*cz);

            if (mod > 0.000001)
            {
                scx += cx;
                scy += cy;
                scz += cz;
            }
        }

        mod = sqrt(scx*scx + scy*scy + scz*scz);
        if (mod > 0.000001)
        {
            normals[3*i] = scx/mod;
            normals[3*i+1] = scy/mod;
            normals[3*i+2] = scz/mod;
        }
        else
        {
            normals[3*i] = scx;
            normals[3*i+1] = scy;
            normals[3*i+2] = scz;
        }
    }
}


/****************************************************************
 * Setters
 * *************************************************************/

void Mesh::setY(const QVector<GLfloat>& yVec, int timeNum)
{
    y    = yVec;
    time = timeNum;
}

void Mesh::setMinY(GLfloat y)
{
    minY = y;
}

void Mesh::setMaxY(GLfloat y)
{
    maxY = y;
}

/****************************************************************
 * Getters
 * *************************************************************/

GLfloat Mesh::getMinY() const
{
    return minY;
}

GLfloat Mesh::getMaxY() const
{
    return maxY;
}

void Mesh::load(const QVector<GLfloat>& xVec,
                const QVector<GLfloat>& yVec,
                const QVector<GLfloat>& zVec,
                int timeNum, GLfloat minX, GLfloat maxX, GLfloat minZ, GLfloat maxZ,
                const QVector<QVector<GLint> >& tris,
                const QVector<QVector<TwoV> >& adjPts,
                int numOfNs, int numOfTs)
{
    GLint vPosition = 0;
    GLint vColor    = 1;
    GLint vNormal   = 2;
    GLint vTex      = 3;

    x               = xVec;
    y               = yVec;
    z               = zVec;
    time            = timeNum;
    minimumX        = minX;
    maxiumX         = maxX;
    minimumZ        = minZ;
    maxiumZ         = maxZ;
    t               = tris;
    p               = adjPts;
    numOfNodes      = numOfNs;
    numOfTriangles  = numOfTs;
    created         = true;

    // no need to change, speed up reload
    if(indices == NULL){
        indices = new GLuint[3*numOfTriangles];
        int indexarraypos = 0;
        for (int i = 0; i < numOfTriangles; i++)
        {
            indices[indexarraypos++] = t[i][0];
            indices[indexarraypos++] = t[i][1];
            indices[indexarraypos++] = t[i][2];
        }
    }

    if(colors == NULL){
        colors  = new GLfloat[4*numOfNodes];
        for (int i = 0; i < numOfNodes; i++)
        {
            colors[4*i]   = 1.0;
            colors[4*i+1] = 1.0;
            colors[4*i+2] = 1.0;
            colors[4*i+3] = 1.0;
        }
    }

    if(tex == NULL){
        tex = new GLfloat[2*numOfNodes];
        GLfloat xDist = maxX - minX;
        GLfloat zDist = maxZ - minZ;

        for (int i = 0; i < numOfNodes; i++)
        {
            tex[2*i]   = (x[i] - minX)/xDist*10;
            tex[2*i+1] = (z[i] - minZ)/zDist*10;
        }
    }

    // change
    if(points == NULL)
        points  = new GLfloat[4*numOfNodes];
    if(normals == NULL)
        normals = new GLfloat[3*numOfNodes];

    for (int i = 0; i < numOfNodes; i++)
    {
        points[4*i]   = x[i];
        points[4*i+1] = y[time+i];
        //qDebug() << points[4*i+1] << " " << y[i];
        points[4*i+2] = z[i];
        points[4*i+3] = 1;
    }

    createNormals();

    if(perturb)
        perturbNormalsRandom(0.1);

    perturb = GL_FALSE;

    if (!reload)
        glGenVertexArrays(1, &vboptr);

    glBindVertexArray(vboptr);

    if (!reload)
        glGenBuffers(1, &eboptr);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboptr);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 3*numOfTriangles*sizeof(GLuint), indices, GL_DYNAMIC_DRAW);

    if (!reload)
        glGenBuffers(1, &bufptr);

    glBindBuffer(GL_ARRAY_BUFFER, bufptr);
    glBufferData(GL_ARRAY_BUFFER, 13*numOfNodes*sizeof(GLfloat),
                 NULL, GL_DYNAMIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, 4*numOfNodes*sizeof(GLfloat), points);
    glBufferSubData(GL_ARRAY_BUFFER, 4*numOfNodes*sizeof(GLfloat),
                    4*numOfNodes*sizeof(GLfloat), colors);
    glBufferSubData(GL_ARRAY_BUFFER, 8*numOfNodes*sizeof(GLfloat),
                    3*numOfNodes*sizeof(GLfloat), normals);
    glBufferSubData(GL_ARRAY_BUFFER, 11*numOfNodes*sizeof(GLfloat),
                    2*numOfNodes*sizeof(GLfloat),
                    tex);


    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
                          BUFFER_OFFSET(0));

    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_TRUE, 0,
                          BUFFER_OFFSET(4*numOfNodes*sizeof(GLfloat)));

    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
                          BUFFER_OFFSET(8*numOfNodes*sizeof(GLfloat)));

    glVertexAttribPointer(vTex, 2, GL_FLOAT, GL_FALSE, 0,
                          BUFFER_OFFSET(11*numOfNodes*sizeof(GLfloat)));

    glEnableVertexAttribArray(vPosition);
    glEnableVertexAttribArray(vColor);
    glEnableVertexAttribArray(vNormal);
    glEnableVertexAttribArray(vTex);
}


void Mesh::reloadData()
{
    reload = GL_TRUE;
    load(x, y, z, time, minimumX, maxiumX, minimumZ, maxiumZ,
         t, p, numOfNodes, numOfTriangles);
}

void Mesh::perturbReloadData()
{
    reload = GL_TRUE;
    perturb = GL_TRUE;
    load(x, y, z, time, minimumX, maxiumX, minimumZ, maxiumZ,
         t, p, numOfNodes, numOfTriangles);
}

void Mesh::draw()
{
    glBindVertexArray(vboptr);
    glDrawElements(GL_TRIANGLES, numOfTriangles, GL_UNSIGNED_INT, NULL);

}

void Mesh::perturbNormalsRandom(float a)
{
    for (int i = 0; i < numOfNodes; i++)
        normals[i] += (double(rand())/double(RAND_MAX)-0.5)*a;

    for (int i = 0; i < numOfNodes; i++)
    {
        GLfloat mod = sqrt(normals[3*i]*normals[3*i] +
                normals[3*i+1]*normals[3*i+1] +
                normals[3*i+2]*normals[3*i+2]);

        if (mod < 0.00001) mod = 0.00001;
        normals[3*i] /= mod;
        normals[3*i+1] /= mod;
        normals[3*i+2] /= mod;
    }
}

bool Mesh::isCreated()
{
    return created;
}
