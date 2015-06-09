#ifndef DETECTOR_H
#define DETECTOR_H

#include <string>
#include <iostream>
#include <vector>
#include <cstdlib>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include <opencv/highgui.h>

#include "../../lib/support/lib_src/cvSupport.h"
#include "../../lib/support/lib_src/support.h"

using namespace cv;
using namespace std;

class Detector
{
public:
    Detector();
    Detector(Mat &image);

    virtual int detect();
    virtual void detection() = 0;

    void setImage(Mat &image);

    Mat getGrayImg();

    Mat getDrawing();

    void setFileName(string filename);
    string getFileName();

    int getFindings();
    void incFindings();
    void drawResults(Mat &img, vector<Rect> &results);
    void drawResult(Rect result);

    Rect maxRect(vector<Rect> & rects);
private:

    int findings;

    string _filename;

    Mat _img;
    Mat _drawing;
    Mat _img_gray;
};

class FaceDetector : public Detector{
#define HAAR_NUM 6
public:
    FaceDetector();

    cv::Rect detectFromImage(cv::Mat &image, cv::Mat &rot);

    int detect();
    int detectface(Mat &img, vector<Rect> &faces);
    virtual void detection();

    int rotetedDetector(cv::Mat &img);// returns the best angle

    cv::Rect avgRect(vector<Rect> &faces);
    cv::Rect medianRect(vector<Rect> &faces);
    cv::Rect avgOf2(Rect a, Rect b);
    int detectAll();
    int detect2Rotate(vector<Rect> &faces, int * angle, int * diff = NULL);


    void detection1();

    void rotator(int boundValue);
    vector<Rect> resultedRects;

private:

    Size approxMinSize();

    void deeperRotator(int resultLeft, int resultRight, int left, int right);

    void saveResult(vector<Rect> & faces, int angle);

    void rotateWithOffset(Mat &img, int angle);

    void rotateResult(int angle, vector<Rect> faces);
    vector<string> _haars;

    vector<CascadeClassifier> FaceClassifiers;


};

#endif // DETECTOR_H
