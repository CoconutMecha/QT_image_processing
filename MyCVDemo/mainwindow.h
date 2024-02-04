#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QLabel>
#include <QMainWindow>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <QToolButton>
#include <QApplication>
#include <QSpinBox>
#include <QTextEdit>
#include <QFileDialog>
#include <opencv2/core/core.hpp>
#include <QDebug>
#include <QTimer>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include "opencv2/imgproc/imgproc_c.h"
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
//usleep
#include <unistd.h>
#include <iostream>




//使用opencv的命名空间
using namespace cv;
QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}

QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //图片居中的声明
    QImage imageCenter(QImage qimage,QLabel *qLabel);
    QImage  MatToQImage(const cv::Mat& mat);
    initConnect();
    void on_btnOpen_file();
    void ReadFrame();
    QString path;
    QTimer * timer;



private slots:

    void on_openPic_clicked();
    void on_action_triggered();
    void on_openVideo_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_clicked();

    void on_openCamera_clicked();

    void on_pushButton_8_clicked();

    void on_action_2_triggered();

    void on_pushButton_5_clicked();

private:
    Ui::MainWindow *ui;


    Mat srcImage;//记录原图的变量，cv命名空间下的Mat类型
    VideoCapture cap;//记录视频的变量
    Mat frame,frame1;//原始视频，处理后的视频灰度化等
    Mat frame_camera,frame_camera1;//摄像头frame,处理后的摄像头frame
    qint16 videoType=0;//视频处理模式 1：灰度化 2：均值滤波 3：边缘检测 4：去雾 5：yolo
    qint16 cameraType=0;//相机处理模式 1：灰度化 2：均值滤波 3：边缘检测 4：去雾 5：yolo


};
#endif // MAINWINDOW_H
