#ifndef LAYERMGR_H
#define LAYERMGR_H

#include <QString>
#include <QVector>
#include "FVCOMDataProcessor.h"
#include "Mesh.h"

class LayerMgr
{
public:
    LayerMgr();
    ~LayerMgr();

    GLfloat getMin(QString varName) const;
    GLfloat getMax(QString varName) const;

    Mesh* getBathy();
    Mesh* getLayer1(const QString& layName);
    Mesh* getLayer2(const QString& layName);
    const QVector<QString>& getVarList() const;

    int getTotalTime() const;
    int update(int change);
    int setFrame(int frame);
    void loadFile(NCLoader loader);

    void checkData();

private:
    QString current1;
    QString current2;
    GLint   numOfNodes;
    GLint   numOfTriangles;
    int     times;
    int     timeCounter;

    QVector<GLfloat>           x;
    QVector<GLfloat>           z;
    QVector<GLfloat>           y1;
    QVector<GLfloat>           y2;
    QVector<GLfloat>           yBathy;
    F1D                        yHash;
    QHash<QString, FloatRange> fRange;
    QVector<QString>           varList;
    QVector<QVector<GLint> >   t;
    QVector<QVector<TwoV> >    adjPts;
    GLfloat                    minX;
    GLfloat                    maxX;
    GLfloat                    minZ;
    GLfloat                    maxZ;

    DataProcessor* dProcessor;

    Mesh meshBathy;
    Mesh mesh1;
    Mesh mesh2;

    void init();
};


#endif // LAYERMGR_H
