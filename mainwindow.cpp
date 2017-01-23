#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include <time.h>
#include "LoadShaders.h"
#include "vec_mat.h"
#include "axes.h"
#include "SphericalCamera.h"
#include "YPRCamera.h"
#include "MaterialLight.h"
#include "Image_File_IO.h"
#include "LayerMgr.h"
#include <QDebug>

/*****************************************************************************
 * Defines
 * **************************************************************************/

#define PI 3.14159265358979323846
#define PI_DIV_180 0.017453292519943296
#define deg PI_DIV_180
#define BUFFER_OFFSET(x) ((const void*) (x))



/****************************************************************************
 * Global Variables
 * **************************************************************************/

// Dirty way to get control of every components in mainwindow
// Global Public UI pointer
Ui::MainWindow *publicUI;

// Global variables
GLint    vPosition;
GLint    vColor;
GLint    vNormal;
GLint    vTex;
GLint    tex1_uniform_loc;
GLuint   program;
GLfloat  camSpeed;
int      numOfLayers;
int      numLights;
GLfloat  ang;
int      frame;
double   et;
int      screenShotCounter;
int      videoShotCounter;
int      videoCounter;

// variables for all layers
QString layName[3];
GLfloat min[3];
GLfloat max[3];
GLfloat transparency[3];
GLfloat yScale[3];
GLfloat yTranslate[3];
GLint   drawOrder[3];


// Camera ID
GLint CameraNumber;

// Texture ID
GLuint texID[2];

// Timer
clock_t timer;
clock_t startTime;
clock_t elapsedTime;
clock_t lastTime;

/*  Set mouse tracking.  */
int mouseDown = 0;
int startX = 0;
int startY = 0;
GLint winWidth;
GLint winHeight;





/****************************************************************************
 * Global Variables - Enum
 *
 * **************************************************************************/
GLenum  mode = GL_FILL;



/****************************************************************************
 * Global Variables - Booleans
 *
 * **************************************************************************/

GLboolean drawAxes       = GL_FALSE;
GLboolean backcolorblack = GL_TRUE;
GLboolean beforeDraw     = GL_TRUE;
//GLboolean afterDraw      = GL_FALSE;
GLboolean var1FirstTime  = GL_TRUE;
GLboolean var2FirstTime  = GL_TRUE;
GLboolean runSimulation  = GL_FALSE;
GLboolean useWaterTexture= GL_TRUE;
GLboolean record         = GL_FALSE;
GLboolean moveSmooth     = GL_TRUE;


GLboolean drawLayer[3];
GLboolean enableLights[3];
GLboolean useEffect[3];
GLboolean enableTexture[3];
GLboolean enableGradientColor[3];
GLboolean enableScale[3];




/****************************************************************************
 * Global Variables - Class Instances and Structures
 *
 * **************************************************************************/

// Axes
axes coords;

// Layers and meshes
LayerMgr      layers;
Mesh*         meshes[3];

// Camera
SphericalCamera sphcamera;
YPRCamera yprcamera;

// Lights
Light Lts[4];

//Matrics
mat4 modelMatrices[3];




/****************************************************************************
 * Global Variables - Matrix and Vectors
 *
 * **************************************************************************/

mat4 projection;
mat4 view;
vec4 GlobalAmbient;




/****************************************************************************
 * OpenGL function prototypes
 *
 * **************************************************************************/
void printOpenGLErrors();
void LoadLights(Light Lt[], int num);
void TurnLightsOn(Light Lt[], int num);
void TurnLightsOff(Light Lt[], int num);
void LoadMaterial(Material Mat);
GLuint loadTextureBMP(const char* filename);
void reshape(GLint width, GLint height);
void SpKeyFcn(int key, int x, int y);
void keyboard(unsigned char key, int x, int y);
void detectUpDown(int button, int state, int x, int y);
void detectMove(int x, int y);
void display();
void update1();
void init();







/****************************************************************************
 * Class member functions
 *
 * **************************************************************************/

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadSucessfully = false;
    ui->actionDraw->setEnabled(false);
    ui->run_simulation_button->setEnabled(false);
    ui->simulation_horizontalSlider->setEnabled(false);
    ui->simulation_lcdNumber->setEnabled(false);
    ui->actionScreen_Shot->setEnabled(false);
    ui->resetCamara_pushButton->setEnabled(false);
    ui->animation_speed_lineEdit->setInputMask("00.0");

    // exposure private ui pointer
    publicUI = ui;
}

MainWindow::~MainWindow()
{
    if(ui) delete ui;
}

void MainWindow::on_actionLoad_file_triggered()
{
    ui->statusBar->showMessage(tr("Loading data..."));
    NCLoader loader;
    if(!loader.loadFile())
        QMessageBox::warning(this, "Error", "Unable to load files");
    else{
        layers.loadFile(loader);
        loadSucessfully = true;
        ui->actionDraw->setEnabled(true);
        ui->actionLoad_file->setEnabled(false);
        QVector<QString> varList = layers.getVarList();
        QString str = "Layer Off";
        ui->varComboBox1->addItem(str, QString());
        ui->varComboBox2->addItem(str, QString());
        for(int i = 0; i < varList.size(); i++){
            ui->varComboBox1->addItem(varList[i], varList[i]);
            ui->varComboBox2->addItem(varList[i], varList[i]);
        }

        // intialize horizontal slider
        ui->simulation_horizontalSlider->setRange(0, layers.getTotalTime());
        ui->simulation_horizontalSlider->setTickInterval(1);
        ui->statusBar->showMessage(tr("Ready"));
        ui->current_camera_value_label->setText(QString::number(CameraNumber));
        vec3 pos = sphcamera.getPosition();
        ui->x_value_label->setText(QString::number(pos.x));
        ui->y_value_label->setText(QString::number(pos.y));
        ui->z_value_label->setText(QString::number(pos.z));
    }
}

void MainWindow::on_actionDraw_triggered()
{
    int argc = 0;
    char* argv = "NCViewr\0";
    glutInit(&argc, &argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(1024, 768);
    glutCreateWindow(argv);

    if (glewInit())
    {
        std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;
        exit(EXIT_FAILURE);
    }

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);
    glutSpecialFunc(SpKeyFcn);
    glutMouseFunc(detectUpDown);
    glutMotionFunc(detectMove);
    glutIdleFunc(update1);

    // setup the main window
    if(layName[0] != "Layer Off"){
        meshes[0] = layers.getLayer1(layName[0]);
        if(ui->var1_perturb_water_normals_checkBox->isChecked())
            meshes[0]->perturbReloadData();
        ui->var1_Data_Min->setText(QString::number(meshes[0]->getMinY()));
        ui->var1_Data_Max->setText(QString::number(meshes[0]->getMaxY()));
        min[0] = meshes[0]->getMinY();
        max[0] = meshes[0]->getMaxY();
        var1FirstTime = GL_FALSE;
        ui->var1_display_min_lineEdit->setText(QString::number(min[0]));
        ui->var1_display_max_lineEdit->setText(QString::number(max[0]));
    }

    if(layName[1] != "Layer Off"){
        meshes[1] = layers.getLayer2(layName[1]);
        if(ui->var2_perturb_water_normals_checkBox->isChecked())
            meshes[1]->perturbReloadData();
        ui->var2_Data_Min->setText(QString::number(meshes[1]->getMinY()));
        ui->var2_Data_Max->setText(QString::number(meshes[1]->getMaxY()));
        min[1] = meshes[1]->getMinY();
        max[1] = meshes[1]->getMaxY();
        var2FirstTime = GL_FALSE;
        ui->var2_display_min_lineEdit->setText(QString::number(min[1]));
        ui->var2_display_max_lineEdit->setText(QString::number(max[1]));
    }

    ui->camera_spinBox->setValue(camSpeed);
    ui->current_camera_value_label->setText(QString::number(CameraNumber));
    ui->var1_3D_Radio_Button->click();
    ui->var2_2D_Radio_Button->click();
    ui->actionDraw->setEnabled(false);
    ui->run_simulation_button->setEnabled(true);
    ui->simulation_horizontalSlider->setEnabled(true);
    ui->simulation_lcdNumber->setEnabled(true);
    ui->actionScreen_Shot->setEnabled(true);
    ui->resetCamara_pushButton->setEnabled(true);
    ui->var1_translateinY_lineEdit->setText(QString::number(yTranslate[0]));
    ui->var2_translateinY_lineEdit->setText(QString::number(yTranslate[1]));
    ui->var1_Scale_spinBox->setValue(yScale[0]);
    ui->var2_Scale_spinBox->setValue(yScale[1]);
    ui->animation_speed_lineEdit->setText("5.0");
    ui->statusBar->showMessage(QString());

    glutMainLoop();
}

void MainWindow::on_actionScreen_Shot_triggered()
{
    if(!beforeDraw){
        display();
        QString fileName = "screenshots//ScreenShot" +
                QString::number(screenShotCounter++) +
                ".bmp";
        WriteBMP(fileName.toStdString().c_str());
    }
}

void MainWindow::on_actionExit_triggered()
{
    if(!beforeDraw){
        glutLeaveMainLoop();
        if(ui)
            delete ui;
    }
    else
        exit (EXIT_SUCCESS);
}

void init()
{
    // for debugging
    // layers.checkData();

    vPosition = 0;
    vColor = 1;
    vNormal = 2;
    vTex = 3;

    numOfLayers = 3;
    numLights = 4;
    ang = 0.0;      /////////////////////////////
    et = 0.2;

    screenShotCounter = 0;
    videoShotCounter  = 1;
    videoCounter      = 1;
    CameraNumber      = 1;
    camSpeed          =  1000.0;
    drawOrder[0]      = 2;

    for(int i = 0; i < numOfLayers; i++){
        min[i] = 0;
        max[i] = 0;
        transparency[i] = 0.5;
        yScale[i]       = 1;
        yTranslate[i]   = 0;

        enableLights[i]        = GL_TRUE;
        useEffect[i]           = GL_FALSE;
        enableTexture[i]       = GL_TRUE;
        enableGradientColor[i] = GL_FALSE;
        enableScale[i]         = GL_FALSE;
    }

    beforeDraw = GL_FALSE;
    moveSmooth = GL_TRUE;

    if(layName[0] != "Layer Off")
        meshes[0] = layers.getLayer1(layName[0]);
    if(layName[1] != "Layer Off")
        meshes[1] = layers.getLayer1(layName[1]);
    //    if(layName[2] != "Layer Off")
    meshes[2] = layers.getBathy();

    // initialize model matrix array
    for (int i = 0; i < numOfLayers; i++)
        modelMatrices[i] = Identity();

    // initialize cameras
    sphcamera.setPosition(50000.0, 40, 60);
    vec3 camPos = sphcamera.getPosition();
    publicUI->x_value_label->setText(QString::number(camPos.x));
    publicUI->y_value_label->setText(QString::number(camPos.y));
    publicUI->z_value_label->setText(QString::number(camPos.z));


    yprcamera.setPosition(40000, 40000, 40000);
    yprcamera.setView(-1, -1, -1);
    yprcamera.setUpViaY();

    coords.load(vPosition, vColor);

    // initialize OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    //  Load the shaders
    ShaderInfo shaders[] =
    {
        {GL_VERTEX_SHADER, "vertexShader.vert"},
        {GL_FRAGMENT_SHADER, "Phong.frag"},
        {GL_NONE, NULL}
    };

    program = LoadShaders(shaders);
    glUseProgram(program);


    // texture settings
    tex1_uniform_loc = glGetUniformLocation(program, "tex1");
    glUniform1i(tex1_uniform_loc, 0);

    glActiveTexture(GL_TEXTURE0);
    texID[0] = loadTextureBMP("textures//seafloor.bmp");
    if (texID[0] == 0) std::cout << "Texture Load Error!" << "\n";

    glActiveTexture(GL_TEXTURE0+1);
    texID[1] = loadTextureBMP("textures//water.bmp");
    if (texID[1] == 0) std::cout << "Texture Load Error!" << "\n";

    // lighting settings
    GlobalAmbient = vec4(0.2, 0.2, 0.2, 1);
    glUniform4fv(glGetUniformLocation(program, "GlobalAmbient"), 1, GlobalAmbient);

    // initialize light array
    float fact = 2;

    numLights = 1;
    for (int i = 0; i < numLights; i++)
    {
        Lts[i] = defaultLight;
        Lts[i].ambient  = fact*Lts[i].ambient;
        Lts[i].diffuse  = fact*Lts[i].diffuse;
        Lts[i].specular = fact*Lts[i].specular;
    }
    // qDebug() << layers.getMin("x") << layers.getMin("z");
    Lts[0].position = vec4(layers.getMin("x")*50, 5000000, layers.getMin("z")*50, 1);
    Lts[1].position = vec4(layers.getMin("x")*50, 5000000, layers.getMax("z")*50, 1);
    Lts[2].position = vec4(layers.getMax("x")*50, 5000000, layers.getMin("z")*50, 1);
    Lts[3].position = vec4(layers.getMax("x")*50, 5000000, layers.getMax("z")*50, 1);

    LoadLights(Lts, numLights);
    glUniform1i(glGetUniformLocation(program, "numLights"), numLights);
}


void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // reset all parameters for axis
    TurnLightsOff(Lts, numLights);
    glUniform1i(glGetUniformLocation(program, "enableGradientColor"), 0);
    glUniform1i(glGetUniformLocation(program, "useTexture"), 0);
    glUniform1i(glGetUniformLocation(program, "enableScale"), 0);

    vec3 camPos;
    if (CameraNumber == 1){
        view = sphcamera.lookAt();
        camPos = sphcamera.getPosition();
    }
    else if (CameraNumber == 2){
        view = yprcamera.lookAt();
        camPos = yprcamera.getPosition();
    }

    if (drawAxes){
        GLfloat camDist = sqrt(camPos.x*camPos.x + camPos.y*camPos.y + camPos.z+camPos.z);
        mat4 axesscale = Scale(camDist/20);
        glUniformMatrix4fv(glGetUniformLocation(program, "ProjModelView"),
                           1, GL_TRUE, projection*view*axesscale);

        coords.draw();
    }


    if (CameraNumber == 1)
        glUniform3fv(glGetUniformLocation(program, "eye"), 1, sphcamera.getPosition());
    else if (CameraNumber == 2)
        glUniform3fv(glGetUniformLocation(program, "eye"), 1, yprcamera.getPosition());


    if(enableLights[0] == GL_TRUE){
        drawOrder[1] = 0;
        drawOrder[2] = 1;
    }
    else{
        drawOrder[1] = 1;
        drawOrder[2] = 0;
    }


    for (int j = 0; j < numOfLayers; j++){
        int i = drawOrder[j];

        if(!drawLayer[i])
            continue;

        glUniformMatrix4fv(glGetUniformLocation(program, "ProjModelView"),
                           1, GL_TRUE, projection*view*modelMatrices[i]);

        glUniformMatrix4fv(glGetUniformLocation(program, "ModelView"),
                           1, GL_TRUE, modelMatrices[i]);

        //        mat3 normal = Adjoint(modelMatrices[i]);
        mat3 normal = Adjoint(Identity());
        glUniformMatrix3fv(glGetUniformLocation(program, "NormalMatrix"),
                           1, GL_FALSE, normal);  //  Note the False for transposing

        glUniform1f(glGetUniformLocation(program, "minY"), min[i]);
        glUniform1f(glGetUniformLocation(program, "maxY"), max[i]);
        glUniform1f(glGetUniformLocation(program, "transparency"), transparency[i]);

        if(enableLights[i]){
            TurnLightsOn(Lts, numLights);

            if(i == 2)
                LoadMaterial(seafloor);
            else if(layName[i] == "zeta")
                LoadMaterial(water);
            else if(layName[i] == "salinity")
                LoadMaterial(redPlastic);
            else
                LoadMaterial(bronze);
        }
        else{
            TurnLightsOff(Lts, numLights);
        }

        if(i == 2 || (layName[i] == "zeta" && enableTexture[i])){
            glUniform1i(glGetUniformLocation(program, "useTexture"), 1);
            GLint textureNum=(i==2)?0:1;
            glUniform1i(tex1_uniform_loc, textureNum);
            if(i == 2)
                glUniformMatrix2fv(glGetUniformLocation(program, "texTrans"),
                                   1, GL_TRUE, Identity2());
            else
                glUniformMatrix2fv(glGetUniformLocation(program, "texTrans"),
                                   1, GL_TRUE, Scale(0.3, 0.3)*Rotate2(ang));
        }
        else
            glUniform1i(glGetUniformLocation(program, "useTexture"), 0);

        if(enableGradientColor[i])
            glUniform1i(glGetUniformLocation(program, "enableGradientColor"), 1);
        else
            glUniform1i(glGetUniformLocation(program, "enableGradientColor"), 0);


        meshes[i]->draw();
    }

    glutSwapBuffers();
    printOpenGLErrors();
}

void update1()
{
    if(!beforeDraw && runSimulation){

        timer = clock();
        double NowSec = (double)(timer - startTime)/CLOCKS_PER_SEC;
        double elapsedTime = (double)(timer - lastTime)/CLOCKS_PER_SEC;

        if (elapsedTime > et)
        {
            if(layName[0] != "Layer Off" || layName[1] != "Layer Off"){

                frame = layers.update(1);
                publicUI->simulation_lcdNumber->display(frame+1);
                publicUI->simulation_horizontalSlider->setValue(frame);
                ang = 1.0/3.0*(2*PI*NowSec);

                for(int i = 0; i < 2; i++){
                    if(layName[i] != "Layer Off"){
                        if(useEffect[i])
                            meshes[i]->perturbReloadData();
                        else
                            meshes[0]->reloadData();
                    }
                }
                if(record){
                    display();
                    QString fileName = "video//videoShot" +
                            QString::number(videoShotCounter++) +
                            ".bmp";
                    WriteBMP(fileName.toStdString().c_str());
                }
            }

            lastTime = timer;
            glutPostRedisplay();
        }
    }
}

void MainWindow::on_varComboBox1_currentIndexChanged(const QString &arg1)
{
    layName[0] = arg1;
    if(arg1 == "Layer Off"){
        drawLayer[0] = GL_FALSE;
    }
    else{
        drawLayer[0] = GL_TRUE;
        if(!beforeDraw){
            meshes[0] = layers.getLayer1(arg1);
            if(ui->var1_perturb_water_normals_checkBox->isChecked())
                meshes[0]->perturbReloadData();
            ui->var1_Data_Min->setText(QString::number(meshes[0]->getMinY()));
            ui->var1_Data_Max->setText(QString::number(meshes[0]->getMaxY()));
            if(var1FirstTime){
                min[0] = meshes[0]->getMinY();
                max[0] = meshes[0]->getMaxY();
                var1FirstTime = GL_FALSE;
            }
            ui->var1_display_min_lineEdit->setText(QString::number(min[0]));
            ui->var1_display_max_lineEdit->setText(QString::number(max[0]));
        }
    }
    if(!beforeDraw){
        glutPostRedisplay();
    }
}

void MainWindow::on_varComboBox2_currentIndexChanged(const QString &arg1)
{
    layName[1] = arg1;
    if(arg1 == "Layer Off"){
        drawLayer[1] = GL_FALSE;
    }
    else{
        drawLayer[1] = GL_TRUE;
        if(!beforeDraw){
            meshes[1] = layers.getLayer2(arg1);
            if(ui->var2_perturb_water_normals_checkBox->isChecked())
                meshes[1]->perturbReloadData();
            ui->var2_Data_Min->setText(QString::number(meshes[1]->getMinY()));
            ui->var2_Data_Max->setText(QString::number(meshes[1]->getMaxY()));
            if(var2FirstTime){
                min[1] = meshes[1]->getMinY();
                max[1] = meshes[1]->getMaxY();
                var2FirstTime = GL_FALSE;
            }
            ui->var2_display_min_lineEdit->setText(QString::number(min[1]));
            ui->var2_display_max_lineEdit->setText(QString::number(max[1]));
        }
    }
    if(!beforeDraw){
        glutPostRedisplay();
    }
}

void MainWindow::on_var1_2D_Radio_Button_clicked(bool checked)
{
    enableLights[0]        = GL_FALSE;
    enableTexture[0]       = GL_FALSE;
    enableGradientColor[0] = GL_TRUE;
    enableScale[0]         = GL_FALSE;

    if(!beforeDraw){
        glutPostRedisplay();
    }
}

void MainWindow::on_var1_3D_Radio_Button_clicked(bool checked)
{
    enableLights[0]        = GL_TRUE;
    enableTexture[0]       = GLboolean(ui->var1_use_water_texture_checkBox->isChecked());
    enableGradientColor[0] = GL_FALSE;
    enableScale[0]         = GL_TRUE;

    if(!beforeDraw){
        glutPostRedisplay();
    }
}

void MainWindow::on_var2_2D_Radio_Button_clicked(bool checked)
{
    enableLights[1]        = GL_FALSE;
    enableTexture[1]       = GL_FALSE;
    enableGradientColor[1] = GL_TRUE;
    enableScale[1]         = GL_FALSE;

    if(!beforeDraw){
        glutPostRedisplay();
    }
}

void MainWindow::on_var2_3D_Radio_Button_clicked(bool checked)
{
    enableLights[1]        = GL_TRUE;
    enableTexture[1]       = GLboolean(ui->var2_use_water_texture_checkBox->isChecked());
    enableGradientColor[1] = GL_FALSE;
    enableScale[1]         = GL_TRUE;

    if(!beforeDraw){
        glutPostRedisplay();
    }
}

void MainWindow::on_var1_Transparent_spinBox_valueChanged(int arg1)
{
    // percentage
    transparency[0] = 0.01 * arg1;

    if(!beforeDraw)
        glutPostRedisplay();
}

void MainWindow::on_var1_Scale_spinBox_valueChanged(int arg1)
{
    yScale[0] = 1.0 * arg1;
    modelMatrices[0] = Translate(0, yTranslate[0], 0) * Scale(1, yScale[0], 1);

    if(!beforeDraw)
        glutPostRedisplay();
}

void MainWindow::on_var2_Transparent_spinBox_valueChanged(int arg1)
{
    // percentage
    transparency[1] = 0.01 * arg1;

    if(!beforeDraw)
        glutPostRedisplay();
}

void MainWindow::on_var2_Scale_spinBox_valueChanged(int arg1)
{
    yScale[1] = 1.0 * arg1;
    modelMatrices[1] = Translate(0, yTranslate[1], 0) * Scale(1, yScale[1], 1);

    if(!beforeDraw)
        glutPostRedisplay();
}

void MainWindow::on_var1_translateinY_lineEdit_textChanged(const QString &arg1)
{
    yTranslate[0] = arg1.toFloat();

    modelMatrices[0] = Translate(0, yTranslate[0], 0) * Scale(1, yScale[0], 1);
    if(!beforeDraw)
        glutPostRedisplay();
}

void MainWindow::on_var2_translateinY_lineEdit_textChanged(const QString &arg1)
{
    yTranslate[1] = arg1.toFloat();
    modelMatrices[1] = Translate(0, yTranslate[1], 0) * Scale(1, yScale[1], 1);
    if(!beforeDraw)
        glutPostRedisplay();
}

void MainWindow::on_seafloor_checkBox_toggled(bool checked)
{
    drawLayer[2] = GLboolean(checked);

    if(!beforeDraw){
        if(meshes[2] == NULL)
            meshes[2] = layers.getBathy();
        glutPostRedisplay();
    }
}

void MainWindow::on_var1_display_min_lineEdit_textChanged(const QString &arg1)
{
    min[0] = arg1.toFloat();
    if(!beforeDraw)
        glutPostRedisplay();
}

void MainWindow::on_var1_display_max_lineEdit_textChanged(const QString &arg1)
{
    max[0] = arg1.toFloat();
    if(!beforeDraw)
        glutPostRedisplay();
}

void MainWindow::on_var2_display_min_lineEdit_textChanged(const QString &arg1)
{
    min[1] = arg1.toFloat();
    if(!beforeDraw)
        glutPostRedisplay();
}

void MainWindow::on_var2_display_max_lineEdit_textChanged(const QString &arg1)
{
    max[1] = arg1.toFloat();
    if(!beforeDraw)
        glutPostRedisplay();
}

void MainWindow::on_camera_spinBox_valueChanged(int arg1)
{
    camSpeed = 1.0 * arg1;
}

void MainWindow::on_run_simulation_button_clicked(bool checked)
{

    if(!beforeDraw){
        if(!runSimulation)
            ui->run_simulation_button->setText("Pause");
        else
            ui->run_simulation_button->setText("Run Simulation");

        runSimulation = !runSimulation;

        glutPostRedisplay();
    }
}

void MainWindow::on_simulation_horizontalSlider_sliderMoved(int position)
{
    ui->simulation_lcdNumber->display(position+1);
    if(!beforeDraw){
        layers.setFrame(position);
        for(int i = 0; i < 2; i++){
            if(layName[i] != "Layer Off" && layName[i] == "zeta"){
                if(useEffect[i])
                    meshes[i]->perturbReloadData();
                else
                    meshes[i]->reloadData();
            }
        }
        glutPostRedisplay();
    }
}

void MainWindow::on_var1_perturb_water_normals_checkBox_clicked(bool checked)
{
    useEffect[0] = GLboolean(checked);
    if(!beforeDraw && layName[0] == "zeta"){
        if(useEffect[0])
            meshes[0]->perturbReloadData();
        else
            meshes[0]->reloadData();

        glutPostRedisplay();
    }
}

void MainWindow::on_var1_use_water_texture_checkBox_clicked(bool checked)
{
    if(!beforeDraw){
        if(!publicUI->var1_2D_Radio_Button->isChecked()){
            enableTexture[0] = GLboolean(checked);
            glutPostRedisplay();
        }
    }
}

void MainWindow::on_var2_perturb_water_normals_checkBox_clicked(bool checked)
{
    useEffect[1] = GLboolean(checked);
    if(!beforeDraw && layName[1] == "zeta"){
        if(useEffect[1])
            meshes[1]->perturbReloadData();
        else
            meshes[1]->reloadData();

        glutPostRedisplay();
    }
}

void MainWindow::on_var2_use_water_texture_checkBox_clicked(bool checked)
{
    if(!beforeDraw){
        if(!publicUI->var2_2D_Radio_Button->isChecked()){
            enableTexture[1] = GLboolean(checked);
            glutPostRedisplay();
        }
    }
}

void MainWindow::on_record_checkBox_clicked(bool checked)
{
    record = GLboolean(checked);
    if(record){
        ui->statusBar->showMessage("Recording..." );
        videoShotCounter = 1;
    }
    if(!record){
        if(videoShotCounter != 1){
            QProcess process;
            QString videoName = QString::number(videoCounter) + ".mp4";
            ui->statusBar->showMessage("Saving " + videoName + "..." );
            QString cmd = "bin//makeVideo.bat " +
                    ui->animation_speed_lineEdit->text() + " " +
                    QString::number(videoCounter++);

            //            QString cmd = "bin\\makeVideo.bat " +
            //                          ui->animation_speed_lineEdit->text() + " " +
            //                          QString::number(videoCounter++);
            //            system(qPrintable(cmd));
            process.start(cmd);
            process.waitForFinished(-1);

            ui->statusBar->showMessage(QString());
        }
    }
}

void MainWindow::on_animation_speed_lineEdit_returnPressed()
{
    QString input = ui->animation_speed_lineEdit->text();
    et = 1.0 / input.toDouble();
}

void MainWindow::on_smooth_move_checkBox_clicked(bool checked)
{
    moveSmooth = GLboolean(checked);
}

void MainWindow::on_resetCamara_pushButton_clicked()
{
    if(!beforeDraw){
        camSpeed = 1000.0;
        sphcamera.setPosition(50000.0, 40, 60);
        yprcamera.setPosition(40000, 40000, 40000);
        yprcamera.setView(-1, -1, -1);
        yprcamera.setUpViaY();
        publicUI->camera_spinBox->setValue(camSpeed);

        vec3 camPos = (CameraNumber==1)?sphcamera.getPosition():yprcamera.getPosition();

        publicUI->x_value_label->setText(QString::number(camPos.x));
        publicUI->y_value_label->setText(QString::number(camPos.y));
        publicUI->z_value_label->setText(QString::number(camPos.z));

        glutPostRedisplay();
    }
}

void printOpenGLErrors()
{
    GLenum errCode;
    const GLubyte *errString;

    while ((errCode = glGetError()) != GL_NO_ERROR)
    {
        errString = gluErrorString(errCode);
        std::cout << "OpenGL Error: " << errString << std::endl;
    }
}

void LoadLights(Light Lt[], int num)
{
    using namespace std;
    string arrayname = "Lt";  // array name in the shader.

    for (int i = 0; i < num; i++)
    {
        char n[10];
        itoa(i, n, 10);
        string arraypos = arrayname;
        arraypos += "[";
        arraypos += n;
        arraypos += "]";

        string locID = arraypos;
        locID.append(".on");
        glUniform1i(glGetUniformLocation(program, locID.c_str()), Lt[i].on);

        locID = arraypos;
        locID.append(".position");
        glUniform4fv(glGetUniformLocation(program, locID.c_str()), 1, Lt[i].position);

        locID = arraypos;
        locID.append(".ambient");
        glUniform4fv(glGetUniformLocation(program, locID.c_str()), 1, Lt[i].ambient);

        locID = arraypos;
        locID.append(".diffuse");
        glUniform4fv(glGetUniformLocation(program, locID.c_str()), 1, Lt[i].diffuse);

        locID = arraypos;
        locID.append(".specular");
        glUniform4fv(glGetUniformLocation(program, locID.c_str()), 1, Lt[i].specular);

        locID = arraypos;
        locID.append(".spotDirection");
        glUniform3fv(glGetUniformLocation(program, locID.c_str()), 1, Lt[i].spotDirection);

        locID = arraypos;
        locID.append(".attenuation");
        glUniform3fv(glGetUniformLocation(program, locID.c_str()), 1, Lt[i].attenuation);

        locID = arraypos;
        locID.append(".spotCutoff");
        glUniform1f(glGetUniformLocation(program, locID.c_str()), Lt[i].spotCutoff);

        locID = arraypos;
        locID.append(".spotExponent");
        glUniform1f(glGetUniformLocation(program, locID.c_str()), Lt[i].spotExponent);
    }
}

void TurnLightsOn(Light Lt[], int num)
{
    using namespace std;
    string arrayname = "Lt";  // array name in the shader.

    for (int i = 0; i < num; i++)
    {
        char n[10];
        itoa(i, n, 10);
        string arraypos = arrayname;
        arraypos += "[";
        arraypos += n;
        arraypos += "]";

        Lt[i].on = true;
        glUniform1i(glGetUniformLocation(program, arraypos.append(".on").c_str()), Lt[i].on);
    }
}

void TurnLightsOff(Light Lt[], int num)
{
    using namespace std;
    string arrayname = "Lt";  // array name in the shader.

    for (int i = 0; i < num; i++)
    {
        char n[10];
        itoa(i, n, 10);
        string arraypos = arrayname;
        arraypos += "[";
        arraypos += n;
        arraypos += "]";

        Lt[i].on = false;
        glUniform1i(glGetUniformLocation(program, arraypos.append(".on").c_str()), Lt[i].on);
    }
}

void LoadMaterial(Material Mat)
{
    glUniform4fv(glGetUniformLocation(program, "Mat.ambient"), 1, Mat.ambient);
    glUniform4fv(glGetUniformLocation(program, "Mat.diffuse"), 1, Mat.diffuse);
    glUniform4fv(glGetUniformLocation(program, "Mat.specular"), 1, Mat.specular);
    glUniform4fv(glGetUniformLocation(program, "Mat.emission"), 1, Mat.emission);
    glUniform1f(glGetUniformLocation(program, "Mat.shininess"), Mat.shininess);
}

GLuint loadTextureBMP(const char* filename)
{
    GLint imgwidth;
    GLint imgheight;
    GLuint texID;

    GLbyte* image1 = LoadBMP(filename, &imgwidth, &imgheight);
    if (!image1) return 0;

    glGenTextures(1, &texID);
    if (texID == 0) return 0;

    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgwidth, imgheight, 0, GL_RGB, GL_UNSIGNED_BYTE, image1);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    free(image1);
    return texID;
}

void reshape(GLint width, GLint height)
{
    winWidth = width;
    winHeight = height;

    glViewport(0, 0, width, height);
    projection = Perspective(60.0f, (float)width/(float)height, 0.0001f, 1000000.0f);
    glutPostRedisplay();
}

void SpKeyFcn(int key, int x, int y)
{
    GLint ctrlDown = glutGetModifiers() & GLUT_ACTIVE_CTRL;
    GLint altDown = glutGetModifiers() & GLUT_ACTIVE_ALT;
    GLint shiftDown = glutGetModifiers() & GLUT_ACTIVE_SHIFT;

    switch (key)
    {
    case GLUT_KEY_F1:
        break;

    case GLUT_KEY_F10:
        //  Since we are double buffering we need
        //  to send the current screen to the back buffer.
        display();
        WriteBMP("ScreenShot.bmp");
        break;

    case GLUT_KEY_F11:
        drawAxes = GL_TRUE;
        break;

    case GLUT_KEY_F12:
        drawAxes = GL_FALSE;
        break;
    }

    vec3 camPos;
    if (CameraNumber == 1)
    {
        GLfloat fact = 0.0001;
        camPos = sphcamera.getPosition();
        GLfloat camDist = sqrt(camPos.x*camPos.x + camPos.y*camPos.y + camPos.z+camPos.z);
        if (ctrlDown)
        {
            switch (key)
            {
            case GLUT_KEY_DOWN:
                if(moveSmooth)
                    sphcamera.addR(-camSpeed*camDist*fact);
                else
                    sphcamera.addR(-camSpeed);
                break;

            case GLUT_KEY_UP:
                if(moveSmooth)
                    sphcamera.addR(camSpeed*camDist*fact);
                else
                    sphcamera.addR(camSpeed);
                break;
            }
        }
        else
        {
            switch (key)
            {
            case GLUT_KEY_UP:
                sphcamera.addPsy(-2);
                break;

            case GLUT_KEY_DOWN:
                sphcamera.addPsy(2);
                break;

            case GLUT_KEY_LEFT:
                sphcamera.addTheta(-2);
                break;

            case GLUT_KEY_RIGHT:
                sphcamera.addTheta(2);
                break;
            }
        }
    }
    else if (CameraNumber == 2)
    {
        GLfloat fact = 0.0001;
        camPos = yprcamera.getPosition();
        GLfloat camDist = sqrt(camPos.y*camPos.y);
        if (ctrlDown)
        {
            switch (key)
            {
            case GLUT_KEY_DOWN:
                if(moveSmooth)
                    yprcamera.moveForward(camSpeed*camDist*fact);
                else
                    yprcamera.moveForward(camSpeed);
                break;

            case GLUT_KEY_UP:
                if(moveSmooth)
                    yprcamera.moveForward(-camSpeed*camDist*fact);
                else
                    yprcamera.moveForward(-camSpeed);
                break;
            }
        }
        else if (altDown)
        {
            switch (key)
            {
            case GLUT_KEY_LEFT:
                yprcamera.addRoll(-1);
                break;

            case GLUT_KEY_RIGHT:
                yprcamera.addRoll(1);
                break;
            }
        }
        else if (shiftDown)
        {
            switch (key)
            {
            case GLUT_KEY_LEFT:
                if(moveSmooth)
                    yprcamera.moveRight(-camSpeed*camDist*fact);
                else
                    yprcamera.moveRight(-camSpeed);
                break;

            case GLUT_KEY_RIGHT:
                if(moveSmooth)
                    yprcamera.moveRight(camSpeed*camDist*fact);
                else
                    yprcamera.moveRight(camSpeed);
                break;

            case GLUT_KEY_UP:
                if(moveSmooth)
                    yprcamera.moveUp(camSpeed*camDist*fact);
                else
                    yprcamera.moveUp(camSpeed);
                break;

            case GLUT_KEY_DOWN:
                if(moveSmooth)
                    yprcamera.moveUp(-camSpeed*camDist*fact);
                else
                    yprcamera.moveUp(-camSpeed);
                break;
            }
        }
        else
        {
            switch (key)
            {
            case GLUT_KEY_UP:
                yprcamera.addPitch(1);
                break;

            case GLUT_KEY_DOWN:
                yprcamera.addPitch(-1);
                break;

            case GLUT_KEY_LEFT:
                yprcamera.addYaw(1);
                break;

            case GLUT_KEY_RIGHT:
                yprcamera.addYaw(-1);
                break;
            }
        }
    }

    publicUI->x_value_label->setText(QString::number(camPos.x));
    publicUI->y_value_label->setText(QString::number(camPos.y));
    publicUI->z_value_label->setText(QString::number(camPos.z));

    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    GLint ctrlDown = glutGetModifiers() & GLUT_ACTIVE_CTRL;
    GLint altDown = glutGetModifiers() & GLUT_ACTIVE_ALT;

    switch(key)
    {
    case 033:  // ASCII Escape Key Code
        exit(EXIT_SUCCESS);
        break;

    case 'm':
        if (mode == GL_FILL)
            mode = GL_LINE;
        else
            mode = GL_FILL;

        glPolygonMode(GL_FRONT_AND_BACK, mode);
        break;

    case 'c':
        if (backcolorblack)
        {
            glClearColor(1, 1, 1, 1);
            backcolorblack = GL_FALSE;
        }
        else
        {
            glClearColor(0, 0, 0, 1);
            backcolorblack = GL_TRUE;
        }
        break;

    case '1':
        CameraNumber = 1;
        break;

    case '2':
        CameraNumber = 2;
        break;
    }

    publicUI->current_camera_value_label->setText(QString::number(CameraNumber));

    glutPostRedisplay();
}

void detectUpDown(int button, int state, int x, int y)
{
    if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
    {
        mouseDown = 1;
        startX = x;
        startY = y;
    }
    else if((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN))
    {
        mouseDown = 2;
        startX = x;
        startY = y;
    }
    else
        mouseDown = 0;
}

void detectMove(int x, int y)
{
    GLint ctrlDown = glutGetModifiers() & GLUT_ACTIVE_CTRL;


    if (mouseDown == 1)
    {
        vec3 camPos;
        if(CameraNumber == 1)
        {
            GLfloat fact = 0.0001;
            camPos = sphcamera.getPosition();
            GLfloat camDist = sqrt(camPos.x*camPos.x + camPos.y*camPos.y + camPos.z+camPos.z);
            if (ctrlDown)
            {
                if(moveSmooth)
                    sphcamera.addR(camSpeed*camDist*fact*(startY-y)/20.0);
                else
                    sphcamera.addR(camSpeed*(startY-y)/20.0);
                startY = y;
            }
            else
            {
                sphcamera.addTheta((x-startX)/2.0);
                startX = x;

                sphcamera.addPsy((y-startY)/2.0);
                startY = y;
            }
            camPos = sphcamera.getPosition();
        }

        if(CameraNumber == 2)
        {
            GLfloat fact = 0.00001;
            camPos = yprcamera.getPosition();
            GLfloat camDist = sqrt(camPos.y*camPos.y);
            if (ctrlDown)
            {
                if(moveSmooth)
                    yprcamera.moveForward(-camSpeed*camDist*fact*(startY-y));
                else
                    yprcamera.moveForward(-camSpeed*(startY-y));
                startY = y;
            }
            else
            {
                if(moveSmooth)
                    yprcamera.moveRight(camSpeed*camDist*fact*(startX-x)/10.0);
                else
                    yprcamera.moveRight(camSpeed*(startX-x)/10.0);
                startX = x;

                if(moveSmooth)
                    yprcamera.moveUp(camSpeed*camDist*fact*(y-startY)/10.0);
                else
                    yprcamera.moveUp(camSpeed*(y-startY)/10.0);
                startY = y;
            }
        }
        publicUI->x_value_label->setText(QString::number(camPos.x));
        publicUI->y_value_label->setText(QString::number(camPos.y));
        publicUI->z_value_label->setText(QString::number(camPos.z));
        glutPostRedisplay();
    }
}
