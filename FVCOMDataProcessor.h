#ifndef FVCOMDATAPROCESSOR_H
#define FVCOMDATAPROCESSOR_H

#include "DataProcessor.h"

class FVCOMDataProcessor:public DataProcessor
{
public:
    FVCOMDataProcessor();
    ~FVCOMDataProcessor();

    void processFloatData();
    void processIntData();
    void findAdjPoints();
    void fakeMapping(const QString& varName, QVector<GLfloat>& v);
};

#endif // FVCOMDATAPROCESSOR_H
