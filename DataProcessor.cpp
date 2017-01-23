#include "DataProcessor.h"

DataProcessor::DataProcessor()
{

}

DataProcessor::~DataProcessor()
{
}


void DataProcessor::useLoader(NCLoader l)
{
    loader = l;
}

void DataProcessor::proceeData()
{
    processFloatData();
    processIntData();
    findAdjPoints();
}

QVector<GLfloat> DataProcessor::getX() const
{
    return x;
}

QVector<GLfloat> DataProcessor::getZ() const
{
    return z;
}

GLint DataProcessor::getTimes () const
{
    return loader.getTimes();
}

F1D DataProcessor::getYHash() const
{
    return yHash;
}

QHash<QString, FloatRange> DataProcessor::getFRange() const
{
    return fRange;
}

QVector<QString> DataProcessor::getVarList() const
{
    return varList;
}

QVector<QVector<GLint> > DataProcessor::getTriangles() const
{
    return triangles;
}

QVector<QVector<TwoV> > DataProcessor::getAdjPoints() const
{
    return adjPoints;
}

void DataProcessor::processFloatData()
{

}

void DataProcessor::processIntData()
{

}

void DataProcessor::findAdjPoints()
{

}

void DataProcessor::fakeMapping(const QString& varName, QVector<GLfloat>& v)
{

}

FloatRange DataProcessor::findRange(const QVector<GLfloat>& v)
{
    FloatRange r;
    r.min = v[0];
    r.max = v[0];

    for(int i = 1; i < v.size(); i++){
        if(r.min > v[i])
            r.min = v[i];
        if(r.max < v[i])
            r.max = v[i];
    }

    return r;
}
