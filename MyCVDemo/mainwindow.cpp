#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "yolo.h"
using namespace std;
using namespace cv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //固定窗口大小，可改为自适应窗口
    setFixedSize(width(),height());
    timer=new QTimer(this);
    initConnect();
    cap = VideoCapture();
}

MainWindow::~MainWindow()
{
    delete ui;

}

MainWindow::initConnect()
{
    connect(timer,&QTimer::timeout,this,&ReadFrame);
}


//点击打开图片按钮
void MainWindow::on_openPic_clicked()
{
    //tr方便跨平台，兼容性
    QString imgFilePath = QFileDialog::getOpenFileName(this,tr("打开图片"),"C:/","(打开图片(*.jpg *.png *.bmp))");
    if(imgFilePath.isEmpty())
    {
        return;
    }

    //Mat类型，可以看作数组，元组
    srcImage = imread(imgFilePath.toStdString());//转化标准字符串
    //把cv图像对象转换为qimage对象
    cvtColor(srcImage,srcImage,CV_BGR2RGB);//输入对象，输出对象（同名表示覆盖原来的），转换的类型
    //Mat转换为Qimage对象
    QImage displayImg = QImage(srcImage.data,srcImage.cols,srcImage.rows,srcImage.cols * srcImage.channels(),QImage::Format_RGB888);//格式化一个8位的
    QImage disimage = imageCenter(displayImg,ui->before_pic_lbl);
    //显示图片到页面
    ui->before_pic_lbl->setPixmap(QPixmap::fromImage(disimage));
}

//图片居中显示，图片大小与label大小相适应
QImage MainWindow::imageCenter(QImage qimage,QLabel *qLabel)
{
    QImage image;
    QSize imageSize = qimage.size();
    QSize labelSize = qLabel->size();

    double dWidthRatio = 1.0 * imageSize.width() / labelSize.width();
    double dHeightRatio = 1.0 * imageSize.height() / labelSize.height();
    if (dWidthRatio>dHeightRatio)
    {
        image = qimage.scaledToWidth(labelSize.width());
    }
    else
    {
        image = qimage.scaledToHeight(labelSize.height());
    }
    return image;
}


//点击打开图片菜单
void MainWindow::on_action_triggered()
{
    //tr方便跨平台，兼容性
    QString imgFilePath = QFileDialog::getOpenFileName(this,tr("打开图片"),"C:/","(打开图片(*.jpg *.png *.bmp))");
    if(imgFilePath.isEmpty())
    {
        return;
    }

    //Mat类型，可以看作数组，元组
    srcImage = imread(imgFilePath.toStdString());//转化标准字符串
    //把cv图像对象转换为qimage对象
    cvtColor(srcImage,srcImage,CV_BGR2RGB);//输入对象，输出对象（同名表示覆盖原来的），转换的类型
    //Mat转换为Qimage对象
    QImage displayImg = QImage(srcImage.data,srcImage.cols,srcImage.rows,srcImage.cols * srcImage.channels(),QImage::Format_RGB888);//格式化一个8位的
    QImage disimage = imageCenter(displayImg,ui->before_pic_lbl);
    //显示图片到页面
    ui->before_pic_lbl->setPixmap(QPixmap::fromImage(disimage));
}

//点击打开视频按钮
void MainWindow::on_openVideo_clicked()
{
    path=QFileDialog::getOpenFileName(this,"打开文件","./","*.*");
    //读取视频
    bool ret=cap.open(path.toStdString().c_str());
    timer->start(50);

}

void MainWindow::ReadFrame()
{ 
    if(cap.isOpened())
    {
        cap.read(frame);
        if(!frame.empty())
        {
            //视频灰度化
            if(videoType==1)
            {
                //qDebug()<<videoType;
                cvtColor(frame,frame1,CV_BGR2GRAY);
                //转换灰度化还要转回来不然会崩溃，和图像处理一样
                 cvtColor(frame1,frame1,CV_GRAY2BGR);
                //cvtColor(frame,frame1,CV_BGR2RGB);
                QImage im1(frame1.data,frame1.cols,frame1.rows,QImage::Format_RGB888);
                this->ui->after_video_lbl->setPixmap(QPixmap::fromImage(im1));
                //自适应大小
                this->ui->after_video_lbl->setScaledContents(true);
                qApp->processEvents();
            }
            //视频均值滤波
            if(videoType==2)
            {
                blur(frame,frame1,Size(4,4));
                QImage im1(frame1.data,frame1.cols,frame1.rows,QImage::Format_RGB888);
                this->ui->after_video_lbl->setPixmap(QPixmap::fromImage(im1));
                //自适应大小
                this->ui->after_video_lbl->setScaledContents(true);
                qApp->processEvents();
            }
            //视频边缘检测
            if(videoType==3)
            {
                Mat frame2;//fram2是处理成灰度 frame1是处理成边缘检测最终结果保存头文件
                //灰度处理
                cvtColor(frame,frame2,CV_BGR2GRAY);
                //调用Canny边缘检测函数
                Canny(frame2,frame1,200,1);
                //转换回BGR类型
                cvtColor(frame1,frame1,CV_GRAY2BGR);
                QImage im1(frame1.data,frame1.cols,frame1.rows,QImage::Format_RGB888);
                this->ui->after_video_lbl->setPixmap(QPixmap::fromImage(im1));
                //自适应大小
                this->ui->after_video_lbl->setScaledContents(true);
                qApp->processEvents();
            }
            //视频增强
            if(videoType==4)
            {
                //hsvFrame 就是frame1和之前的类似;
                cvtColor(frame, frame1, COLOR_BGR2HSV);

                // 提取亮度通道（V）并进行直方图均衡化
                vector<Mat> channels;
                split(frame1, channels);
                equalizeHist(channels[2]+20, channels[2]);

                // 合并通道并转换回RGB色彩空间
                merge(channels, frame1);
                cvtColor(frame1, frame1, COLOR_HSV2BGR);
                QImage im1(frame1.data,frame1.cols,frame1.rows,QImage::Format_RGB888);
                this->ui->after_video_lbl->setPixmap(QPixmap::fromImage(im1));
                //自适应大小
                this->ui->after_video_lbl->setScaledContents(true);
                qApp->processEvents();
            }

            //yolo-fast
            if(videoType==5)
            {
                qDebug()<<"目标检测";
                YOLO yolo_model(yolo_nets[0]);
                //Mat srcimg = srcImage;
                yolo_model.detect(frame);
                cvtColor(frame, frame1, CV_BGR2RGB);
                QImage im1(frame1.data,frame1.cols,frame1.rows,QImage::Format_RGB888);
                this->ui->after_video_lbl->setPixmap(QPixmap::fromImage(im1));
                //自适应大小
                this->ui->after_video_lbl->setScaledContents(true);
                qApp->processEvents();
            }

            cvtColor(frame,frame,CV_BGR2RGB);
            QImage im(frame.data,frame.cols,frame.rows,QImage::Format_RGB888);
            //qDebug()<<frame.data;
            this->ui->before_video_lbl->setPixmap(QPixmap::fromImage(im));
            //自适应大小
            this->ui->before_video_lbl->setScaledContents(true);
            qApp->processEvents();
        }
    }
}






//点击灰度化
void MainWindow::on_pushButton_4_clicked()
{
    //获取当前tab的序号
    QString currentTabName = ui->showArea->currentWidget()->objectName();
    //QMessageBox::information(NULL, "Title",currentTabName,QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);测试
    if(currentTabName=="pictureArea")
    {
        //实现灰度化
        Mat resultImage;
        cvtColor(srcImage,resultImage,COLOR_BGR2GRAY);
        //QImage对象必须是三原色对象，要显示需要处理一下,转换回来
        cvtColor(resultImage,resultImage,COLOR_GRAY2BGR);
        QImage displayImg = QImage(resultImage.data,resultImage.cols,resultImage.rows,resultImage.cols * resultImage.channels(),QImage::Format_RGB888);//格式化一个8位的
        QImage disimage = imageCenter(displayImg,ui->after_pic_lbl);
        //显示图片到页面
        ui->after_pic_lbl->setPixmap(QPixmap::fromImage(disimage));
    }
    else if(currentTabName=="videoArea")
    {
        videoType = 1;
    }
    else if(currentTabName=="cameraArea")
    {
        cameraType = 1 ;
    }

}

//点击边缘检测按钮
void MainWindow::on_pushButton_7_clicked()
{
    //获取当前tab的序号
    QString currentTabName = ui->showArea->currentWidget()->objectName();
    //QMessageBox::information(NULL, "Title",currentTabName,QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);测试
    if(currentTabName=="pictureArea")
    {
        Mat edgeImage,grayImage;
        //灰度处理
        cvtColor(srcImage,grayImage,COLOR_BGR2GRAY);
        //调用Canny边缘检测函数
        Canny(grayImage,edgeImage,200,1);
        //转换回BGR类型
        cvtColor(edgeImage,edgeImage,COLOR_GRAY2BGR);
        QImage displayImg = QImage(edgeImage.data,edgeImage.cols,edgeImage.rows,edgeImage.cols * edgeImage.channels(),QImage::Format_RGB888);//格式化一个8位的
        QImage disimage = imageCenter(displayImg,ui->after_pic_lbl);
        //显示图片到页面
        ui->after_pic_lbl->setPixmap(QPixmap::fromImage(disimage));
    }
     if(currentTabName=="videoArea")
    {
        videoType = 3;
    }
     if(currentTabName=="cameraArea")
     {
         cameraType = 3;
     }
}

//点击均值滤波按钮
void MainWindow::on_pushButton_clicked()
{
    //获取当前tab的序号
    QString currentTabName = ui->showArea->currentWidget()->objectName();
    //QMessageBox::information(NULL, "Title",currentTabName,QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);测试
    if(currentTabName=="pictureArea")
    {
        Mat blurImg;
        //调用均值滤波方法
        blur(srcImage,blurImg,Size(4,4));
        QImage displayImg = QImage(blurImg.data,blurImg.cols,blurImg.rows,blurImg.cols * blurImg.channels(),QImage::Format_RGB888);//格式化一个8位的
        QImage disimage = imageCenter(displayImg,ui->after_pic_lbl);
        //显示图片到页面
        ui->after_pic_lbl->setPixmap(QPixmap::fromImage(disimage));
    }
    if(currentTabName=="videoArea")
    {
        videoType = 2;
    }
    if(currentTabName=="cameraArea")
    {
        cameraType=2;
    }
}

//点击打开摄像头
void MainWindow::on_openCamera_clicked()
{

    cv::VideoCapture cap(0); // 打开默认摄像头（索引号为0）
    if (!cap.isOpened()) {
        qDebug() << "无法连接到摄像头";
        return;
    }

    while (true) {
        // 从摄像头获取当前帧并转换为RGB格式
        cap >> frame_camera;
        //qDebug()<<cameraType;

        //灰度化
        if(cameraType==1)
        {
            qDebug()<<cameraType;
            cvtColor(frame_camera,frame_camera1,CV_BGR2GRAY);
            //转换灰度化还要转回来不然会崩溃，和图像处理一样
            cvtColor(frame_camera1,frame_camera1,CV_GRAY2BGR);
            // 将OpenCV图像数据复制到Qt图片对象中
            QImage image((uchar*)frame_camera1.data, frame_camera1.cols, frame_camera1.rows, QImage::Format_RGB888);

            // 设置标签控件的图像

            this->ui->after_camera_lbl->setPixmap(QPixmap::fromImage(image));
            //自适应大小
            this->ui->after_camera_lbl->setScaledContents(true);
            qApp->processEvents();

            // 等待一定时间后再次处理新的帧
            usleep(30000); // 每秒30帧
        }
        else if(cameraType==2)
        {
            blur(frame_camera,frame_camera1,Size(4,4));
            // 将OpenCV图像数据复制到Qt图片对象中
            QImage image((uchar*)frame_camera1.data, frame_camera1.cols, frame_camera1.rows, QImage::Format_RGB888);

            // 设置标签控件的图像

            this->ui->after_camera_lbl->setPixmap(QPixmap::fromImage(image));
            //自适应大小
            this->ui->after_camera_lbl->setScaledContents(true);
            qApp->processEvents();

            // 等待一定时间后再次处理新的帧
            usleep(30000); // 每秒30帧
        }
        else if(cameraType==3)
        {
            Mat frame_camera2;//fram2是处理成灰度 frame1是处理成边缘检测最终结果保存头文件
            //灰度处理
            cvtColor(frame_camera,frame_camera2,CV_BGR2GRAY);
            //调用Canny边缘检测函数
            Canny(frame_camera2,frame_camera1,200,1);
            //转换回BGR类型
            cvtColor(frame_camera1,frame_camera1,CV_GRAY2BGR);
            // 将OpenCV图像数据复制到Qt图片对象中
            QImage image((uchar*)frame_camera1.data, frame_camera1.cols, frame_camera1.rows, QImage::Format_RGB888);

            // 设置标签控件的图像

            this->ui->after_camera_lbl->setPixmap(QPixmap::fromImage(image));
            //自适应大小
            this->ui->after_camera_lbl->setScaledContents(true);
            qApp->processEvents();

            // 等待一定时间后再次处理新的帧
            usleep(30000); // 每秒30帧
        }
        //摄像机图像增强
        else if(cameraType==4)
        {
            //hsvFrame 就是frame1和之前的类似;
            cvtColor(frame_camera,frame_camera1, COLOR_BGR2HSV);

            // 提取亮度通道（V）并进行直方图均衡化
            vector<Mat> channels;
            split(frame_camera1, channels);
            equalizeHist(channels[2]+20, channels[2]);

            // 合并通道并转换回RGB色彩空间
            merge(channels, frame_camera1);
            cvtColor(frame_camera1, frame_camera1, COLOR_HSV2BGR);

            // 将OpenCV图像数据复制到Qt图片对象中
            QImage image((uchar*)frame_camera1.data, frame_camera1.cols, frame_camera1.rows, QImage::Format_RGB888);

            // 设置标签控件的图像

            this->ui->after_camera_lbl->setPixmap(QPixmap::fromImage(image));
            //自适应大小
            this->ui->after_camera_lbl->setScaledContents(true);
            qApp->processEvents();

            // 等待一定时间后再次处理新的帧
            usleep(30000); // 每秒30帧
        }
        else if(cameraType==5)
        {
            qDebug()<<"摄像机yolo";
            YOLO yolo_model(yolo_nets[0]);
            //Mat srcimg = srcImage;
            cvtColor(frame_camera, frame_camera1, cv::COLOR_BGR2RGB);
            //frame_camera1 = frame_camera;
            yolo_model.detect(frame_camera1);
            cvtColor(frame_camera1, frame_camera1, cv::COLOR_BGR2RGB);

            // 将OpenCV图像数据复制到Qt图片对象中
            QImage image((uchar*)frame_camera1.data, frame_camera1.cols, frame_camera1.rows, QImage::Format_RGB888);

            // 设置标签控件的图像

            this->ui->after_camera_lbl->setPixmap(QPixmap::fromImage(image));
            //自适应大小
            this->ui->after_camera_lbl->setScaledContents(true);
            qApp->processEvents();

            // 等待一定时间后再次处理新的帧
            usleep(30000); // 每秒30帧

        }



        cv::cvtColor(frame_camera, frame_camera, cv::COLOR_BGR2RGB);

        // 将OpenCV图像数据复制到Qt图片对象中
        QImage image((uchar*)frame_camera.data, frame_camera.cols, frame_camera.rows, QImage::Format_RGB888);

        // 设置标签控件的图像

        this->ui->before_camera_lbl->setPixmap(QPixmap::fromImage(image));
        //自适应大小
        this->ui->before_camera_lbl->setScaledContents(true);
        qApp->processEvents();

        // 等待一定时间后再次处理新的帧
        usleep(30000); // 每秒30帧
    }

}

//点击图像增强按钮(去雾，增加清晰度)
void MainWindow::on_pushButton_8_clicked()
{
    //获取当前tab的序号
    QString currentTabName = ui->showArea->currentWidget()->objectName();
    //QMessageBox::information(NULL, "Title",currentTabName,QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);测试
    if(currentTabName=="pictureArea")
    {
        if (srcImage.empty()) {
            std::cout << "无法打开输入图像" << std::endl;
            return;
        }

        // 将图像转换为HSV色彩空间
        Mat hsvImage;
        cvtColor(srcImage, srcImage, COLOR_BGR2HSV);

        // 提取亮度通道（V）并进行直方图均衡化
        vector<Mat> channels;
        split(srcImage, channels);
        equalizeHist(channels[2]+20, channels[2]);

        // 合并通道并转换回RGB色彩空间
        merge(channels, hsvImage);
        cvtColor(hsvImage, srcImage, COLOR_HSV2BGR);

        // 显示结果图像
        QImage displayImg = QImage(srcImage.data,srcImage.cols,srcImage.rows,srcImage.cols * srcImage.channels(),QImage::Format_RGB888);//格式化一个8位的
        QImage disimage = imageCenter(displayImg,ui->after_pic_lbl);
        //显示图片到页面
        ui->after_pic_lbl->setPixmap(QPixmap::fromImage(disimage));
    }
    //视频增强
    if(currentTabName=="videoArea")
    {
        videoType = 4;
    }
    if(currentTabName=="cameraArea")
    {
        cameraType=4;
    }
}






//菜单打开视频
void MainWindow::on_action_2_triggered()
{
    path=QFileDialog::getOpenFileName(this,"打开文件","./","*.*");
    //读取视频
    bool ret=cap.open(path.toStdString().c_str());
    timer->start(50);
}

//点击目标检测按钮
void MainWindow::on_pushButton_5_clicked()
{
    //获取当前tab的序号
    QString currentTabName = ui->showArea->currentWidget()->objectName();
    if(currentTabName=="pictureArea")
    {
        //待完成 已完成
        YOLO yolo_model(yolo_nets[0]);
        //Mat srcimg = srcImage;
        yolo_model.detect(srcImage);
        cvtColor(srcImage,srcImage,CV_BGR2RGB);//输入对象，输出对象（同名表示覆盖原来的），转换的类型
        //Mat转换为Qimage对象
        QImage displayImg = QImage(srcImage.data,srcImage.cols,srcImage.rows,srcImage.cols * srcImage.channels(),QImage::Format_RGB888);//格式化一个8位的
        QImage disimage = imageCenter(displayImg,ui->after_pic_lbl);
        //显示图片到页面
        ui->after_pic_lbl->setPixmap(QPixmap::fromImage(disimage));
    }
    //检测视频
    else if(currentTabName=="videoArea")
    {
        videoType = 5;
    }
    //检测摄像机
    else if(currentTabName=="cameraArea")
    {
        cameraType = 5;
        //qDebug()<<"摄像机检测1";
        qDebug()<<cameraType<<currentTabName;
    }


}

//以下是yolo-faster部分代码

YOLO::YOLO(Net_config config)
{
    cout << "Net use " << config.netname << endl;
    this->confThreshold = config.confThreshold;
    this->nmsThreshold = config.nmsThreshold;
    this->inpWidth = config.inpWidth;
    this->inpHeight = config.inpHeight;
    strcpy_s(this->netname, config.netname.c_str());

    ifstream ifs(config.classesFile.c_str());
    string line;
    while (getline(ifs, line)) this->classes.push_back(line);

    this->net = readNetFromDarknet(config.modelConfiguration, config.modelWeights);
    this->net.setPreferableBackend(DNN_BACKEND_OPENCV);
    this->net.setPreferableTarget(DNN_TARGET_CPU);
}

void YOLO::postprocess(Mat& frame, const vector<Mat>& outs)   // Remove the bounding boxes with low confidence using non-maxima suppression
{
    vector<int> classIds;
    vector<float> confidences;
    vector<Rect> boxes;

    for (size_t i = 0; i < outs.size(); ++i)
    {
        // Scan through all the bounding boxes output from the network and keep only the
        // ones with high confidence scores. Assign the box's class label as the class
        // with the highest score for the box.
        float* data = (float*)outs[i].data;
        for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
        {
            Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
            Point classIdPoint;
            double confidence;
            // Get the value and location of the maximum score
            minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
            if (confidence > this->confThreshold)
            {
                int centerX = (int)(data[0] * frame.cols);
                int centerY = (int)(data[1] * frame.rows);
                int width = (int)(data[2] * frame.cols);
                int height = (int)(data[3] * frame.rows);
                int left = centerX - width / 2;
                int top = centerY - height / 2;

                classIds.push_back(classIdPoint.x);
                confidences.push_back((float)confidence);
                boxes.push_back(Rect(left, top, width, height));
            }
        }
    }

    // Perform non maximum suppression to eliminate redundant overlapping boxes with
    // lower confidences
    vector<int> indices;
    NMSBoxes(boxes, confidences, this->confThreshold, this->nmsThreshold, indices);
    for (size_t i = 0; i < indices.size(); ++i)
    {
        int idx = indices[i];
        Rect box = boxes[idx];
        this->drawPred(classIds[idx], confidences[idx], box.x, box.y,
                       box.x + box.width, box.y + box.height, frame);
    }
}

void YOLO::drawPred(int classId, float conf, int left, int top, int right, int bottom, Mat& frame)   // Draw the predicted bounding box
{
    //Draw a rectangle displaying the bounding box
    rectangle(frame, Point(left, top), Point(right, bottom), Scalar(0, 0, 255), 3);

    //Get the label for the class name and its confidence
    string label = format("%.2f", conf);
    if (!this->classes.empty())
    {
        CV_Assert(classId < (int)this->classes.size());
        label = this->classes[classId] + ":" + label;
    }

    //Display the label at the top of the bounding box
    int baseLine;
    Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
    top = max(top, labelSize.height);
    //rectangle(frame, Point(left, top - int(1.5 * labelSize.height)), Point(left + int(1.5 * labelSize.width), top + baseLine), Scalar(0, 255, 0), FILLED);
    putText(frame, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 255, 0), 1);
}

void YOLO::detect(Mat& frame)
{
    Mat blob;
    blobFromImage(frame, blob, 1 / 255.0, Size(this->inpWidth, this->inpHeight), Scalar(0, 0, 0), true, false);
    this->net.setInput(blob);
    vector<Mat> outs;
    this->net.forward(outs, this->net.getUnconnectedOutLayersNames());
    this->postprocess(frame, outs);

    vector<double> layersTimes;
    double freq = getTickFrequency() / 1000;
    double t = net.getPerfProfile(layersTimes) / freq;
    string label = format("%s Inference time : %.2f ms", this->netname, t);
    putText(frame, label, Point(0, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
    //imwrite(format("%s_out.jpg", this->netname), frame);
}

