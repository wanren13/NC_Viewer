#include "NCLoader.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

NCLoader::NCLoader():
times(0)
{

}

NCLoader::~NCLoader()
{
}

bool NCLoader::loadFile()
{
    /************************************************************************
     * Load model infomation
     *
     * times           - number of time intervals
     * *********************************************************************/

    QFile infoFile("dataFiles//info.txt");

    //file failed to open
    if(!infoFile.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

    //open up a stream to read
    QTextStream inInfo(&infoFile);

    QString line      = inInfo.readLine();
    times             = line.toInt();

    infoFile.close();


    QVector<QVector<GLint> > _2di;
    //////////////////////////////////////////////////////////////////////////
    //load triangles
    QFile triFile("dataFiles//vt.txt");

    //file failed to open
    if(!triFile.open(QIODevice::ReadOnly | QIODevice::Text))   return false;

    QTextStream inTri(&triFile);

    int i = 0;
    while(!inTri.atEnd())
    {
        //read one line
        QString line = inTri.readLine();
        QStringList split = line.split(" ");
        QVector<GLint> _1di;
        _1di.append(split.at(0).toInt()-1);
        _1di.append(split.at(1).toInt()-1);
        _1di.append(split.at(2).toInt()-1);
        _2di.append(_1di);
        i++;
    }
    triFile.close();

    _2didata.insert("triangles", _2di);
    _2di.clear();


    /************************************************************************
     * Load coordinates
     *
     * x - longitude
     * y - latitude
     * *********************************************************************/

    QVector<GLfloat > _1df;
    //////////////////////////////////////////////////////////////////////////
    //load longitude
    QFile lonFile("dataFiles//lon.txt");

    //file failed to open
    if(!lonFile.open(QIODevice::ReadOnly | QIODevice::Text))   return false;

    QTextStream inLon(&lonFile);

    while(!inLon.atEnd())
    {
        //read one line
        QString line = inLon.readLine();
        _1df.append(line.toFloat());
    }

    lonFile.close();
    _1dfdata.insert("x", _1df);
    _1df.clear();



    //////////////////////////////////////////////////////////////////////////
    //load latitude
    QFile latFile("dataFiles//lat.txt");

    //file failed to open
    if(!latFile.open(QIODevice::ReadOnly | QIODevice::Text))   return false;

    QTextStream inLat(&latFile);

    while(!inLat.atEnd())
    {
        //read one line
        QString line = inLat.readLine();
        _1df.append(line.toFloat());
    }

    latFile.close();

    _1dfdata.insert("z", _1df);
    _1df.clear();



    /************************************************************************
     * Load float type data
     *
     * bathy    - bathymetric elevation
     * zeta     - sea surface level
     * temp     - temperature
     * salinity - salinity
     * *********************************************************************/


    //////////////////////////////////////////////////////////////////////////
    //load bathy
    QFile bathyFile("dataFiles//h.txt");

    //file failed to open
    if(!bathyFile.open(QIODevice::ReadOnly | QIODevice::Text))   return false;

    QTextStream inBathy(&bathyFile);

    while(!inBathy.atEnd())
    {
        //read one line
        QString line = inBathy.readLine();
        _1df.append(-1.0 * line.toFloat());
    }

    bathyFile.close();

    _1dfdata.insert("bathy", _1df);
    _1df.clear();


    //////////////////////////////////////////////////////////////////////////
    //load zeta
    QFile zetaFile("dataFiles//zeta.txt");

    //file failed to open
    if(!zetaFile.open(QIODevice::ReadOnly | QIODevice::Text))   return false;

    QTextStream inZeta(&zetaFile);

    while(!inZeta.atEnd())
    {
        //read one line
        QString line = inZeta.readLine();
        _1df.append(line.toFloat());
    }

    zetaFile.close();

    _1dfdata.insert("zeta", _1df);
    _1df.clear();



    //////////////////////////////////////////////////////////////////////////
    //load temp
    QFile tempFile("dataFiles//temp.txt");

    //file failed to open
    if(!tempFile.open(QIODevice::ReadOnly | QIODevice::Text))   return false;

    QTextStream inTemp(&tempFile);

    while(!inTemp.atEnd())
    {
        //read one line
        QString line = inTemp.readLine();
        _1df.append(line.toFloat());
    }

    tempFile.close();

    _1dfdata.insert("temp", _1df);
    _1df.clear();


    //////////////////////////////////////////////////////////////////////////
    //load salinity
    QFile saFile("dataFiles//salinity.txt");

    //file failed to open
    if(!saFile.open(QIODevice::ReadOnly | QIODevice::Text))   return false;

    QTextStream inSa(&saFile);

    while(!inSa.atEnd())
    {
        //read one line
        QString line = inSa.readLine();
        _1df.append(line.toFloat());
    }

    saFile.close();

    _1dfdata.insert("salinity", _1df);
    _1df.clear();

    return true;
}

GLint NCLoader::getTimes() const
{
    return times;
}

F1D NCLoader::get_1DFData() const
{
    return _1dfdata;
}

F2D NCLoader::get_2DFData() const
{
    return _2dfdata;
}

I1D NCLoader::get_1DIData() const
{
    return _1didata;
}

I2D NCLoader::get_2DIData() const
{
    return _2didata;
}
