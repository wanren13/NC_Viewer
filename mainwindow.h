#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

private slots:
    void on_actionLoad_file_triggered();
    void on_actionDraw_triggered();
    void on_varComboBox1_currentIndexChanged(const QString &arg1);
    void on_varComboBox2_currentIndexChanged(const QString &arg1);
    void on_var1_2D_Radio_Button_clicked(bool checked);
    void on_var1_3D_Radio_Button_clicked(bool checked);
    void on_var2_2D_Radio_Button_clicked(bool checked);
    void on_var2_3D_Radio_Button_clicked(bool checked);
    void on_var1_Transparent_spinBox_valueChanged(int arg1);
    void on_var1_Scale_spinBox_valueChanged(int arg1);
    void on_var2_Transparent_spinBox_valueChanged(int arg1);
    void on_var2_Scale_spinBox_valueChanged(int arg1);
    void on_seafloor_checkBox_toggled(bool checked);
    void on_camera_spinBox_valueChanged(int arg1);
    void on_var1_display_min_lineEdit_textChanged(const QString &arg1);
    void on_var1_display_max_lineEdit_textChanged(const QString &arg1);
    void on_var2_display_min_lineEdit_textChanged(const QString &arg1);
    void on_var2_display_max_lineEdit_textChanged(const QString &arg1);
    void on_var1_perturb_water_normals_checkBox_clicked(bool checked);
    void on_var1_use_water_texture_checkBox_clicked(bool checked);
    void on_var2_perturb_water_normals_checkBox_clicked(bool checked);
    void on_var2_use_water_texture_checkBox_clicked(bool checked);
    void on_run_simulation_button_clicked(bool checked);
    void on_simulation_horizontalSlider_sliderMoved(int position);
    void on_record_checkBox_clicked(bool checked);
    void on_animation_speed_lineEdit_returnPressed();
    void on_var1_translateinY_lineEdit_textChanged(const QString &arg1);
    void on_var2_translateinY_lineEdit_textChanged(const QString &arg1);
    void on_actionScreen_Shot_triggered();
    void on_actionExit_triggered();
    void on_smooth_move_checkBox_clicked(bool checked);
    void on_resetCamara_pushButton_clicked();

private:
    Ui::MainWindow *ui;
//    QProcess  process;
    bool      loadSucessfully;
};

#endif // MAINWINDOW_H
