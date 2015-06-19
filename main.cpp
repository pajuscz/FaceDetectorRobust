#include <iostream>

#include "lib_src/cvSupport.h"
#include "lib_src/support.h"

#include <vector>

#include <opencv/highgui.h>

#include <boost/filesystem.hpp>

#include "detector.h"


#include "lib_stasm/stasm_lib.h"
#include "lib_stasm/stasm_landmarks.h"

#include "faceDesc/facedescription.h"

using namespace std;

cv::Point get_point_landmark(int index, float * landmarks){
    return cv::Point(landmarks[index*2],landmarks[index*2+1]);
}


void saveMouth(){

}

void saveEyes(){

}

void showStasm(float *stasmLandmarks, string name, Mat & img){
        Mat model;
        img.copyTo(model);

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

cv::Mat cutObject(Mat & _img, DoublePoint & object, int bbox, int bboxup = 0){
    // copy of the image
    Mat img;
    _img.copyTo(img);

    // find the rotation angle
    double angle = cvSupport::angleBottom(object.getLPoint(), object.getRPoint());
    // rotate the image
    Mat rotM = cvSupport::rotateImg(img,angle,object.getCenter());

    object.rotate(rotM);

    cv::Point org = cv::Point(object.getLPoint());
    cv::Point end = cv::Point(object.getRPoint());


    double d = bbox;

    if(bboxup != 0){
        double dd = bboxup;
        //check if points are in the image !!!
        if(org.x-d < 0 || org.x-d > img.cols || org.y-dd < 0 || org.y-dd > img.rows
                || end.x+dd < 0 || end.y+dd < 0 || end.y+d > img.rows || end.x+d > img.cols){
            cout << "Point out of bounds: " << endl;
        }
        else{
            cv::Rect rect(Point(org.x - d, org.y - dd),
                          Point(end.x + d, end.y + dd));
            cv::Mat img_rect = cv::Mat(img,rect);

            return img_rect;
        }
    }
    //check if points are in the image !!!
    if(org.x-d < 0 || org.x-d > img.cols || org.y-d < 0 || org.y-d > img.rows
            || end.x+d < 0 || end.y+d < 0 || end.y+d > img.rows || end.x+d > img.cols){
        cout << "Point out of bounds: " << endl;
    }
    else{
        cv::Rect rect(Point(org.x - d, org.y - d),
                      Point(end.x + d, end.y + d));
        cv::Mat img_rect = cv::Mat(img,rect);

        return img_rect;
    }
    return Mat();
}


FaceState getStasmPoints(float * landmarks, cv::Mat & img){
    DoublePoint mouth(get_point_landmark(L_LMouthCorner,landmarks), get_point_landmark(L_RMouthCorner,landmarks));
    DoublePoint lEye(get_point_landmark(L_LEyeOuter,landmarks), get_point_landmark(L_LEyeInner,landmarks));
    DoublePoint rEye(get_point_landmark(L_REyeInner,landmarks), get_point_landmark(L_REyeOuter,landmarks));

    FaceState newFs(Eye(lEye.getLPoint(),lEye.getRPoint()),
                    Eye(rEye.getLPoint(),rEye.getRPoint()),
                    MouthSimple(mouth.getLPoint(), mouth.getRPoint()));
    return newFs;
}


#define MOUTHS "OUT/Mouths/"
#define EYES "OUT/Eyes/"

void performStasm(cv::Mat img, string anot){
    cv::Mat_<unsigned char> img_gray;
    cvtColor(img,img_gray,CV_BGR2GRAY);


    int foundface;
    float stasmLandmarks[2 * stasm_NLANDMARKS]; // x,y coordinates

    if(!stasm_search_single(&foundface, stasmLandmarks, (char*)img_gray.data, img_gray.cols, img_gray.rows, "", "./lib_stasm/haars/")){
        cerr << "Error in stasm_search_single: " << stasm_lasterr() << endl;
    }
    if(foundface > 0){
        // Load anot file to get states
        FaceState fs;
        fs.openFromFile(anot);
        // load face from stasm
        FaceState found = getStasmPoints(stasmLandmarks,img);

        Mat mouth = cutObject(img,found.mouth, found.mouth.distance()/5, found.mouth.distance()/3 + found.mouth.distance()/6);
        Mat leye = cutObject(img,found.lEye, found.lEye.distance()/2.5);
        Mat reye = cutObject(img,found.rEye, found.rEye.distance()/2.5);

        //SAVING
        string lEyePath = EYES+Support::getFileName(anot) + "_LE_" + fs.lEye.state() + ".jpg";
        string rEyePath = EYES+Support::getFileName(anot) + "_RE_" + fs.rEye.state() + ".jpg";
        string mouthPath = MOUTHS+Support::getFileName(anot) + "_M_" + fs.mouth.getStrState() + ".jpg";

        if(!boost::filesystem::is_directory(EYES)){
            boost::filesystem::create_directories(EYES);
        }
        if(!boost::filesystem::is_directory(MOUTHS)){
            boost::filesystem::create_directories(MOUTHS);
        }

        //if(!boost::filesystem::exists(lEyePath.c_str()) && !boost::filesystem::exists(rEyePath.c_str()) && !boost::filesystem::exists(mouthPath.c_str())){
                imwrite(lEyePath,leye);
                imwrite(rEyePath,reye);
                imwrite(mouthPath,mouth);
                cout << "saving: " << lEyePath << ", " << rEyePath << ", " << mouthPath << endl;
        //    }

    }
    else{
        cerr << anot << " No faces found" << endl;
    }
}

int main(int argc, char ** argv)
{

    /**
      Example takes a directory with images .jpg
      **/
    if(argc > 1){
        vector<string> images = Support::pathVector(argv[1],".jpg");
        vector<string> anots = Support::pathVector(argv[1],".txt");

        sort(images.begin(), images.end());
        sort(anots.begin(), anots.end());
        //Init face detector
        FaceDetector *fd = new FaceDetector();

        // for each image in directory
        for(uint i = 0; i < images.size() ; ++i){
                // read image
                Mat img = imread(images[i]);
                cv::Mat rot;
                Rect  faceRect = fd->detectFromImage(img,rot);
                cv::Mat face = fd->getImg();

                // STASM
                performStasm(face, anots[i]);

               // imshow(Support::getFilePath(images[i]), face);
               // cvSupport::indexBrowser(i,images.size());

              //  fd->saveCroppedFace(Support::getFilePath(images[i]) + Support::getFileName(images[i])+"_face.jpg");
         }
                delete fd;
    } // if argc > 1
    return 0;
}

