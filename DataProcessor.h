#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include "NCLoader.h"
#include "util.h"

struct FloatRange
{
    GLfloat min;
    GLfloat max;
};

class DataProcessor
{
public:
    DataProcessor();
    ~DataProcessor();

    void useLoader(NCLoader l);
    void proceeData();

    QVector<GLfloat> getX() const;
    QVector<GLfloat> getZ() const;
    F1D getYHash() const;
    QHash<QString, FloatRange> getFRange() const;
    QVector<QString> getVarList() const;
    QVector<QVector<GLint> > getTriangles() const;
    QVector<QVector<TwoV> > getAdjPoints() const;
    GLint getTimes () const;
protected:
    NCLoader loader;

    QVector<GLfloat>           x;
    QVector<GLfloat>           z;
    F1D                        yHash;
    QHash<QString, FloatRange> fRange;
    QVector<QString>           varList;
    QVector<QVector<GLint> >   triangles;
    QVector<QVector<TwoV> >    adjPoints;       // set of ajacent points

    virtual void processFloatData();
    virtual void processIntData();
    virtual void findAdjPoints();
    virtual void fakeMapping(const QString& varName, QVector<GLfloat>& v);

    FloatRange findRange(const QVector<GLfloat>& v);
};


#endif // DATAPROCESSOR_H
