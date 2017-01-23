#include "FVCOMDataProcessor.h"
#include <QDebug>

// distance between longitudes and latitudes
// (assume distances are even)
#define DIST 6000.0

FVCOMDataProcessor::FVCOMDataProcessor():DataProcessor()
{

}

FVCOMDataProcessor::~FVCOMDataProcessor()
{
}

void FVCOMDataProcessor::processFloatData()
{
    F1D _1df = loader.get_1DFData();

    F1D::const_iterator i = _1df.constBegin();
    while (i != _1df.constEnd()){
        QString          name = i.key();
        QVector<GLfloat> v    = i.value();

        fRange.insert(name, findRange(v));

        if(name == "x"){
            x = v;
            fakeMapping("x", x);
        }
        else if(name == "z"){
            z = v;
            fakeMapping("z", z);
        }
        else{
            if(name != "bathy")
            varList.append(name);
            yHash.insert(name, v);
        }

        ++i;
    }
}

void FVCOMDataProcessor::processIntData()
{
    I2D _2di = loader.get_2DIData();

    triangles = _2di["triangles"];
}

void FVCOMDataProcessor::findAdjPoints()
{
    adjPoints.resize(x.size());

    for(int i = 0; i < triangles.size(); i++){
        for(int j = 0; j < 3; j++){
            int  p1, p2;

            switch (j) {
            case 0:
                p1 = triangles[i][j+1];
                p2 = triangles[i][j+2];
                break;
            case 1:
                p1 = triangles[i][j-1];
                p2 = triangles[i][j+1];
                break;
            case 2:
                p1 = triangles[i][j-1];
                p2 = triangles[i][j-2];
                break;
            }

            int  p = triangles[i][j];

            adjPoints[p].append({p1, p2});
        }
    }
}

void FVCOMDataProcessor::fakeMapping(const QString& varName, QVector<GLfloat>& v)
{
    GLfloat min    = fRange[varName].min;
    GLfloat max    = fRange[varName].max;
    GLfloat center = (min + max) / 2;

    for(int i = 0; i < v.size(); i++)
        v[i] = (v[i] - center) * DIST;

    fRange[varName].min = (min - center) * DIST;
    fRange[varName].max = (max - center) * DIST;
}

