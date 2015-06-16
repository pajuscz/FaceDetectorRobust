#ifndef FACEDESCRIPTION_H
#define FACEDESCRIPTION_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "lib_src/cvSupport.h"

#include <string>

#include <iostream>


using namespace std;
using namespace cv;


#define FEATURES 4
#define MOUTH_STATES 6

static const string M_O = "OPEN";
static const string M_C = "CLOSED";
static const string M_T = "TRUE";
static const string M_F = "FALSE";


static const string MOUTH_STATE_STR[] = {"DEFAULT", "SMILE", "TONGUE", "TEETH", "OPEN", "OPEN2"};

class DoublePoint{
public:
    DoublePoint();
    DoublePoint(cv::Point left, cv::Point right);
    void setL_XY(int x, int y);
    void setR_XY(int x, int y);
    void setPointL_XY(Point left);
    void setPointR_XY(Point right);

    void rotate(Mat m);
    void draw(cv::Mat img);

    Point getLPoint();
    Point getRPoint();

    Point getCenter();

    double distance();
private:
    Point left,right;
};

class Eye: public DoublePoint{
public:
    Eye();
    Eye(bool open_state);
    Eye(Point l, Point r, bool open_state);
    Eye(Point l, Point r);
    void setOpen(bool open);
    void setState(string state);
    bool getState();

    string state();
private:

    bool open;
    //! Eye stripe calculation
};

class MouthSimple: public DoublePoint{
public:

    MouthSimple();
    MouthSimple(Point l, Point r);
    // Inherited

    void setState(int statenum);
    void setState(string state);
    int getState();
    string getStrState();

    // Just void - Don't Use!
    void setOpen(bool o);
    bool Teeth,Tongue,Smile;

private:
    int state;
};

class FaceState{
public:

    FaceState();
    FaceState(Eye leye, Eye reye, MouthSimple mouth);

    void openFromFile(string filepath);
    void openFromFileRaw(string filepath, string imgPath);

    Eye lEye;
    Eye rEye;
    cv::Point nose;
    MouthSimple mouth;
    double tilt;
    int qtilt;
    enum MOUTH_STATE {DEFAULT, SMILE, TONGUE, TEETH, OPEN, OPEN2};
    enum Features{EYELO,EYERO,EYESO,EYESC};
};
#endif // FACEDESCRIPTION_H
