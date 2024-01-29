#ifndef YOLO_H
#define YOLO_H

#endif // YOLO_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace dnn;
using namespace std;

struct Net_config
{
    float confThreshold; // Confidence threshold
    float nmsThreshold;  // Non-maximum suppression threshold
    int inpWidth;  // Width of network's input image
    int inpHeight; // Height of network's input image
    string classesFile;
    string modelConfiguration;
    string modelWeights;
    string netname;
};

class YOLO
{
public:
    YOLO(Net_config config);
    void detect(Mat& frame);
private:
    float confThreshold;
    float nmsThreshold;
    int inpWidth;
    int inpHeight;
    char netname[20];
    vector<string> classes;
    Net net;
    void postprocess(Mat& frame, const vector<Mat>& outs);
    void drawPred(int classId, float conf, int left, int top, int right, int bottom, Mat& frame);
};

Net_config yolo_nets[1] = {
    {0.5, 0.4, 320, 320,"F:/QT/qtproject/opencvproj/MyCVDemo/yolo_fast/coco.names", "F:/QT/qtproject/opencvproj/MyCVDemo/yolo_fast/yolo-fastest-xl.cfg", "F:/QT/qtproject/opencvproj/MyCVDemo/yolo_fast/yolo-fastest-xl.weights", "yolov-fast"}
    //{0.5, 0.4, 608, 608,"coco.names", "yolov4/yolov4.cfg", "yolov4/yolov4.weights", "yolov4"},1
    //{0.5, 0.4, 320, 320,"coco.names", "yolo-fastest/yolo-fastest-xl.cfg", "yolo-fastest/yolo-fastest-xl.weights", "yolo-fastest"},2
    //{0.5, 0.4, 320, 320,"coco.names", "yolobile/csdarknet53s-panet-spp.cfg", "yolobile/yolobile.weights", "yolobile"}3
};
