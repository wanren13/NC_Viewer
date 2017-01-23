#ifndef NCLOADER_H
#define NCLOADER_H

#include <QVector>
#include "util.h"

class NCLoader
{
public:

    NCLoader();
    ~NCLoader();

    bool loadFile();
    GLint getTimes() const;

    F1D get_1DFData() const;
    F2D get_2DFData() const;
    I1D get_1DIData() const;
    I2D get_2DIData() const;
private:
    GLint times;

    F1D _1dfdata;
    F2D _2dfdata;
    I1D _1didata;
    I2D _2didata;
};

#endif // NCLOADER_H
