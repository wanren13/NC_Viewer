#include "LayerMgr.h"

///////////////////////////////////////////////////////////////
#include <QDebug>
#include <QString>
#include <QMessageBox>

LayerMgr::LayerMgr():
    times(0), timeCounter(0), current1(QString()), current2(QString()),
    dProcessor(NULL)
{

}

LayerMgr::~LayerMgr()
{
}

/***************************************************************************
 * LoadFile function open and process the file
 *
 * The code should be refactored once the NetCDF library has been installed.
 * The program should have a button to pop up open file dialog windowbe and
 * be able to load the NetCDF file which is specified by the user.
 * ************************************************************************/

void LayerMgr::loadFile(NCLoader loader)
{
    dProcessor = new FVCOMDataProcessor();
    dProcessor->useLoader(loader);
    init();
}

void LayerMgr::init()
{
    dProcessor->proceeData();

    x              = dProcessor->getX();
    z              = dProcessor->getZ();
    times          = dProcessor->getTimes();
    yHash          = dProcessor->getYHash();
    fRange         = dProcessor->getFRange();
    varList        = dProcessor->getVarList();
    t              = dProcessor->getTriangles();
    adjPts         = dProcessor->getAdjPoints();

    if(dProcessor)   delete dProcessor;

    numOfNodes     = x.size();
    numOfTriangles = t.size();
    minX           = fRange["x"].min;
    maxX           = fRange["x"].max;
    minZ           = fRange["z"].min;
    maxZ           = fRange["z"].max;
}

Mesh* LayerMgr::getBathy()
{
    yBathy = yHash["bathy"];

    if(!meshBathy.isCreated()){
        meshBathy.load(x, yBathy, z, 0, minX, maxX, minZ, maxZ,
                       t, adjPts, numOfNodes, numOfTriangles);
        meshBathy.setMinY(fRange["bathy"].min);
        meshBathy.setMaxY(fRange["bathy"].max);
    }

    return &meshBathy;
}

Mesh* LayerMgr::getLayer1(const QString& layName)
{
    y1 = yHash[layName];
    int startPos = timeCounter*numOfNodes;

    if (!mesh1.isCreated()){
        mesh1.load(x, y1, z, startPos, minX, maxX, minZ, maxZ,
                   t, adjPts, numOfNodes, numOfTriangles);
        mesh1.setMinY(fRange[layName].min);
        mesh1.setMaxY(fRange[layName].max);
    }
    else {
        if (current1 != layName){
            mesh1.setY(y1, startPos);
            mesh1.reloadData();
            mesh1.setMinY(fRange[layName].min);
            mesh1.setMaxY(fRange[layName].max);
        }
    }

    current1 = layName;

    return &mesh1;
}

Mesh* LayerMgr::getLayer2(const QString& layName)
{
    y2 = yHash[layName];
    int startPos = timeCounter*numOfNodes;

    if (!mesh2.isCreated()){
        mesh2.load(x, y2, z, startPos, minX, maxX, minZ, maxZ,
                   t, adjPts, numOfNodes, numOfTriangles);
        mesh2.setMinY(fRange[layName].min);
        mesh2.setMaxY(fRange[layName].max);
    }
    else {
        if (current2 != layName){

            mesh2.setY(y2, startPos);
            mesh2.reloadData();
            mesh2.setMinY(fRange[layName].min);
            mesh2.setMaxY(fRange[layName].max);
        }
    }

    current2 = layName;

    return &mesh2;
}

int LayerMgr::getTotalTime() const
{
    return times;
}

int LayerMgr::update(int change)
{
    timeCounter += change;

    if(timeCounter < 0)
        timeCounter = times - 1;
    else if(timeCounter == times)
        timeCounter = 0;

    int time = timeCounter*numOfNodes;

    if(mesh1.isCreated()){
        mesh1.setY(y1, time);
        mesh1.reloadData();
    }
    if(mesh2.isCreated()){
        mesh2.setY(y2, time);
        mesh2.reloadData();
    }

    return timeCounter;
}

int LayerMgr::setFrame(int frame)
{
    timeCounter = frame;

    int time = timeCounter*numOfNodes;

    if(mesh1.isCreated()){
        mesh1.setY(y1, time);
        mesh1.reloadData();
    }
    if(mesh2.isCreated()){
        mesh2.setY(y2, time);
        mesh2.reloadData();
    }

    return timeCounter;
}

GLfloat LayerMgr::getMin(QString varName) const
{
    return fRange[varName].min;
}

GLfloat LayerMgr::getMax(QString varName) const
{
    return fRange[varName].max;
}

const QVector<QString>& LayerMgr::getVarList() const
{
    return varList;
}

/*****************************************************************
 *  This function for debugging only
 * ***************************************************************/

void LayerMgr::checkData()
{
    qDebug() << "NumOfNodes: " << numOfNodes;
    qDebug() << "NumofTriangles" << numOfTriangles;
    qDebug() << "times: " << times;

    /*
    qDebug() << "Lon:";
    for(int i=0; i<numOfNodes; i++)
        qDebug() << x.at(i);

    //*/


    /*
    qDebug() << "Lat:";
    for(int i=0; i<numOfNodes; i++)
        qDebug() << z.at(i);


    //*/

    /*
    qDebug() << "Triangles:";
    for(int i=0; i<numOfTriangles; i++){
        qDebug() << t[i][0] << ", "
                            << t[i][1] << ", "
                            << t[i][2];
    }
    //*/


    /*
    qDebug() << "Bathy:";
    qDebug() << "Min: " << fRange["bathy"].min
             << " Max: " << fRange["bathy"].max;
    for(int i=0; i<numOfNodes; i++)
        qDebug() << yHash["bathy"].at(i);

    //*/


    /*
    qDebug() << "zeta:";
    qDebug() << "Min: " << fRange["zeta"].min
             << " Max: " << fRange["zeta"].max;
    for(int i=0; i< numOfNodes*times; i++)
        qDebug() << yHash["zeta"].at(i);
    //*/


    /*
    qDebug() << "temp:";
    qDebug() << "Min: " << fRange["temp"].min
             << " Max: " << fRange["temp"].max;
    for(int i=0; i<numOfNodes*times; i++)
        qDebug() << yHash["temp"].at(i);
    //*/


    /*
    qDebug() << "salinity:";
    qDebug() << "Min: " << fRange["salinity"].min
             << " Max: " << fRange["salinity"].max;
    for(int i=0; i<numOfNodes*times; i++)
        qDebug() << yHash["salinity"].at(i);
    //*/

    /*
    qDebug() << "Points-Edge:";

    for(int i=0; i<adjPts.size(); i++)
    {
        QString str;
        str = "{";
        for(int j=0; j<adjPts[i].size(); j++){
            str = str + "(" + QString::number(adjPts[i][j].v1) + ","
                    + QString::number(adjPts[i][j].v2) + ") ";
        }
        str = str + "}";
        qDebug () << str;
    }
    //*/
}
