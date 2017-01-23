#-------------------------------------------------
#
# Project created by QtCreator 2014-03-19T09:51:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NCViewer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    LoadShaders.cpp \
    SphericalCamera.cpp \
    YPRCamera.cpp \
    Mesh.cpp \
    NCLoader.cpp \
    axes.cpp \
    LayerMgr.cpp \
    DataProcessor.cpp \
    FVCOMDataProcessor.cpp

HEADERS  += mainwindow.h \
    Image_File_IO.h \
    LoadShaders.h \
    MaterialLight.h \
    SphericalCamera.h \
    vec_mat.h \
    YPRCamera.h \
    Mesh.h \
    NCLoader.h \
    axes.h \
    LayerMgr.h \
    DataProcessor.h \
    util.h \
    FVCOMDataProcessor.h

FORMS    += mainwindow.ui

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lfreeglut
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lfreeglut
else:unix: LIBS += -L$$PWD/lib/ -lfreeglut

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/libfreeglut.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/libfreeglut.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/freeglut.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/freeglut.lib
else:unix: PRE_TARGETDEPS += $$PWD/lib/libfreeglut.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lfreeglut_static
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lfreeglut_static
else:unix: LIBS += -L$$PWD/lib/ -lfreeglut_static

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/libfreeglut_static.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/libfreeglut_static.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/freeglut_static.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/freeglut_static.lib
else:unix: PRE_TARGETDEPS += $$PWD/lib/libfreeglut_static.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lglew32
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lglew32
else:unix: LIBS += -L$$PWD/lib/ -lglew32

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/libglew32.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/libglew32.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/glew32.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/glew32.lib
else:unix: PRE_TARGETDEPS += $$PWD/lib/libglew32.a

OTHER_FILES += \
    Phong.frag \
    vertexShader.vert \
    dataFiles/h.txt \
    dataFiles/info.txt \
    dataFiles/lat.txt \
    dataFiles/lon.txt \
    dataFiles/salinity.txt \
    dataFiles/temp.txt \
    dataFiles/vt.txt \
    dataFiles/zeta.txt

RESOURCES += \
    resource.qrc
