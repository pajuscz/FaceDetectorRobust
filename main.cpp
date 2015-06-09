#include <iostream>

#include "lib_src/cvSupport.h"
#include "lib_src/support.h"

#include <vector>

#include <opencv/highgui.h>

#include "detector.h"

#include "libflandmark/flandmark_detector.h"

#include "lib_stasm/stasm_lib.h"
#include "lib_stasm/stasm_landmarks.h"
using namespace std;

cv::Point get_point_landmark(int index, float * landmarks){
    return cv::Point(landmarks[index*2],landmarks[index*2+1]);
}

void performStasm(cv::Mat img, string name){
    cv::Mat_<unsigned char> img_gray;
    cvtColor(img,img_gray,CV_BGR2GRAY);


    int foundface;
    float stasmLandmarks[2 * stasm_NLANDMARKS]; // x,y coordinates


    if(!stasm_search_single(&foundface, stasmLandmarks, (char*)img_gray.data, img_gray.cols, img_gray.rows, "", "./lib_stasm/haars/")){
        cerr << "Error in stasm_search_single: " << stasm_lasterr() << endl;
    }

    Mat model;
        img_gray.copyTo(model);

        // Jaw
        for(int i = 0; i < 12; ++i){
            circle(model,Point(get_point_landmark(i,stasmLandmarks)),1,cv::Scalar(0,100,255),2);
        }
        // Forehead
        for(int i = 12; i < 16; ++i){
            circle(model,Point(get_point_landmark(i, stasmLandmarks)),1,cv::Scalar(0,0,255),2);
        }
        // EyeBrows
        for(int i = 16; i < 28; ++i){
            circle(model,Point(get_point_landmark(i, stasmLandmarks)),1,cv::Scalar(200,0,255),2);
        }
        // Eyes
        for(int i = 28; i < 48; ++i){
            circle(model,Point(get_point_landmark(i, stasmLandmarks)),1,cv::Scalar(255,128,0),2);
        }
        // Nose
        for(int i = 48; i < 59; ++i){
            circle(model,Point(get_point_landmark(i, stasmLandmarks)),1,cv::Scalar(50,255,0),2);
        }
        // Mouth
        for(int i = 59; i < stasm_NLANDMARKS; ++i){
            circle(model,Point(get_point_landmark(i, stasmLandmarks)),1,cv::Scalar(0,50,255),2);
        }



    imshow(name,model);
}

int main(int argc, char ** argv)
{

    /**
      Example takes a directory with images .jpg
      **/
    if(argc > 1){
        vector<string> images = Support::pathVector(argv[1],".jpg");
        cout << argv[1] << endl;
        //Init face detector
        FaceDetector *fd = new FaceDetector();

        // for each image in directory
        for(uint i = 0; i < images.size() ; ++i){
                // read image
                Mat img = imread(images[i]);
                cv::Mat rot;
                Rect  faceRect = fd->detectFromImage(img,rot);
                cv::Mat face = fd->getImg();

                // LOAD FLANDMARK
                FLANDMARK_Model * fModel = flandmark_init("./libflandmark/flandmark_model.dat");
                // load input image

                IplImage *iplimg = new IplImage(face);

                cout << faceRect << endl;
                // convert image to grayscale
                IplImage *img_grayscale = cvCreateImage(cvSize(iplimg->width, iplimg->height), IPL_DEPTH_8U, 1);
                cvCvtColor(iplimg, img_grayscale, CV_BGR2GRAY);
                // bbox with detected face (format: top_left_col top_left_row bottom_right_col bottom_right_row)
                int bbox[] = {faceRect.tl().x, faceRect.tl().y, faceRect.br().x, faceRect.br().y};
                // detect facial landmarks (output are x, y coordinates of detected landmarks)
                double * landmarks = (double*)malloc(2*fModel->data.options.M*sizeof(double));
                flandmark_detect(img_grayscale, bbox, fModel, landmarks);

                cout << landmarks[0] << endl;

                Mat imgfl;
                fd->getCroppedImg().copyTo(imgfl);
                for(int i = 0; i < fModel->data.options.M*2;i+=2){
                    circle(imgfl, Point(landmarks[i] - faceRect.tl().x, landmarks[i+1] - faceRect.tl().y),2,Scalar(0,0,255),2);
                }
                imshow("imgfl",imgfl);

                // STASM
                performStasm(face,"full");
                performStasm(fd->getCroppedImg(),"face");

                imshow(Support::getFilePath(images[i]), face);
                cvSupport::indexBrowser(i,images.size());

              //  fd->saveCroppedFace(Support::getFilePath(images[i]) + Support::getFileName(images[i])+"_face.jpg");
         }
                delete fd;
    } // if argc > 1
    return 0;
}

