#include <iostream>

#include "lib_src/cvSupport.h"
#include "lib_src/support.h"

#include <vector>

#include <opencv/highgui.h>

#include "detector.h"
using namespace std;

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
                fd->detectFromImage(img,rot);

                imshow(Support::getFilePath(images[i]), fd->getCroppedImg());
                cvSupport::indexBrowser(i,images.size());

              //  fd->saveCroppedFace(Support::getFilePath(images[i]) + Support::getFileName(images[i])+"_face.jpg");
         }
                delete fd;
    } // if argc > 1
    return 0;
}

