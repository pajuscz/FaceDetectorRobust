#include <iostream>

#include "../../lib/support/lib_src/cvSupport.h"
#include "../../lib/support/lib_src/support.h"

#include <vector>

#include <opencv/highgui.h>

#include "detector.h"
using namespace std;

int main(int argc, char ** argv)
{

    if(argc == 2){
        if(String(argv[1]) == "-v"){ //video
            VideoCapture cap(0);
            if(!cap.isOpened()){
                cerr << "No camera found" << endl;
                return -1;
            }

            Mat frame;
            Mat drawing;

            FaceDetector *fd = new FaceDetector();

            fd->setFileName("Camera");

            vector<Rect> faces;
            for(;;){
                cap >> frame;
                // Get time before processing
                int64 A = getTickCount();
                // >>> Processing

                fd->setImage(frame);
                fd->detection();

                // Get Time after processing
                int64 B = getTickCount();
                B = B - A;
                // Time result
                double time = B * (1/getTickFrequency()) * 1000;
                float fps = 1000/time;
                String processTime = "frameProcessed: " + to_string(time) + " ms";


                drawing = fd->getDrawing();
                putText(drawing,processTime,Point(0, frame.rows - 20), CV_FONT_NORMAL, 0.8,Scalar(255,255,255));
                putText(drawing,to_string(fps) + " fps",Point(0, frame.rows - 5), CV_FONT_HERSHEY_PLAIN, 0.8,Scalar(255,255,255));
                // ^^^

                imshow("video",drawing);
                // Quit on 'q'
                if( (waitKey(10) == 'q') ) break;
            }
        }
        else{
            vector<string> images = Support::pathVector(argv[1],".jpg");
            //cvSupport::imageBrowser(images);

            FaceDetector *fd = new FaceDetector();

            for(uint i = 0; i < images.size() ; ++i){



                Mat img = imread(images[i]);
                // add borders for rotating the image
//                Mat borderImg = Mat::zeros(700,640,CV_8UC3);
//                img.copyTo(borderImg(Rect(80,30,img.cols,img.rows)));
//                borderImg.copyTo(img);


                int wrap = 0;

                if(wrap == 0){
                    cv::Mat rot;
                    cv::Rect frect = fd->detectFromImage(img,rot);
                    rectangle(img,frect,Scalar(0,0,255),2);
                    fd->setImage(img);
                }
                else{
                    if(img.data){

                        fd->setFileName(Support::getFileName(images[i],true));

                        // Get time before processing
                        int64 A = getTickCount();
                        // >>> Processing
                        //! @TODO load only grayscale image


                        Mat tmp_img;
                        img.copyTo(tmp_img);

                        // Find the best angle
                        int bestAngle = 0;
                        int results = 0;
                        for(int r = -45; r <= 45; r+= 5){
                            cvSupport::rotateImg(tmp_img, r, Point(tmp_img.cols/2, tmp_img.rows/2));
                            fd->setImage(tmp_img);
                            int detects = fd->detectAll();
                            if(detects > results){
                                bestAngle = r;
                                results = detects;
                            }
                            //rectangle(img, fd->avgRect(fd->resultedRect), cvSupport::COLORS[1], 2);
                            //cout << "detected in " << r << "° " << fd->detectAll() << endl;
                            img.copyTo(tmp_img);
                        }

                        if(results > 0){
                            cvSupport::rotateImg(img, bestAngle, Point(tmp_img.cols/2, tmp_img.rows/2));
                            fd->setImage(img);
                            fd->detectAll();

                            Rect faceRect = fd->avgOf2(fd->avgRect(fd->resultedRects), fd->medianRect(fd->resultedRects));
                            //rectangle(img, faceRect, cvSupport::COLORS[6], 2);
                            fd->setImage(img);

                            cout << "Best Angle: " << bestAngle << endl;


                            Mat img_face = Mat::zeros(img.rows,img.cols,CV_8UC3);
                            img(faceRect).copyTo(img_face);

                            fd->setImage(img_face);
                        }

                        // Get Time after processing
                        int64 B = getTickCount();
                        B = B - A;
                        // ^^^

                        // Time result
                        double time = B * (1/getTickFrequency());
                        cout << "Processing time: " << time*1000 << " ms" << " detections: " << fd->getFindings() << endl;
                        // ^^^

                    }
                }

                imshow(Support::getFilePath(images[i]), fd->getDrawing());
                cvSupport::indexBrowser(i,images.size());

            }

            delete fd;
        }

    }
    else{
        cerr << "Bad commandline arguments" << endl;
    }
    return 0;
}

