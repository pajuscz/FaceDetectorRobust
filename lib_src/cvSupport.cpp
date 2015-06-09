#include "cvSupport.h"


vector<uchar> cvSupport::pixsUnderLine(Mat &image, Point A, Point B){
    vector<uchar> pixsLine;

    // do a line segment in tmp_image
    Mat_<uchar> mask = Mat::zeros(image.size(), CV_8UC1);
    line(mask,A,B,Scalar(255));

    // create tmp_image
    Mat_<uchar> img_tmp;
    image.copyTo(img_tmp);

    for(int y = 0; y < img_tmp.rows; ++y){
        for(int x = 0; x < img_tmp.cols; ++x){
            if( mask(y,x) == 255){
                pixsLine.push_back(img_tmp(y,x));
            }
        }
    }

    // Apply Mask
    img_tmp.setTo(0);
    image.copyTo(img_tmp,mask);

    return pixsLine;
}

int cvSupport::numOfSegments(vector<uchar> &line){

    int number = 0;
    bool hasStart = false;
    bool hasEnd = false;

    for(int i = 0; i < line.size(); ++i){
        if(line[i] == 255){
            hasStart = true;
        }

        if(hasStart && line[i] == 0){
            hasStart = false;
            number++;
        }
    }
    return number;

}

int cvSupport::numberOf(vector<uchar> &line, uchar pix){
    int cnt = 0;
    for(int i = 0; i < line.size(); ++i){
        if( line[i] == pix){
            cnt++;
        }
    }
    return cnt;
}

Mat cvSupport::rotateImg(Mat &img, double angle, Point center){

    // if empty center point = rotate around center of the image
    if(center.x == 0 && center.y == 0){
        center = Point(img.cols/2, img.rows/2);
    }

    Mat rot_mat = getRotationMatrix2D(center,angle,1.0); // 1.0 = no scale;

    warpAffine(img,img,rot_mat,img.size());

    return rot_mat;
}

Mat cvSupport::translateImg(Mat &img, int offsetx, int offsety){
    Mat trans_mat = (Mat_<double>(2,3) << 1, 0, offsetx, 0, 1, offsety);
    warpAffine(img,img,trans_mat,img.size());
    return trans_mat;
}

Point cvSupport::rotatePoint2D(Point p, Mat M){
    Point r;
    r.x = M.at<double>(0,0) * p.x + M.at<double>(0,1)*p.y + M.at<double>(0,2);
    r.y = M.at<double>(1,0) * p.x + M.at<double>(1,1)*p.y + M.at<double>(1,2);
// @ TODO why -(..) ? prolly because of sign creation of head tilt
    if(r.x < 0) r.x = -r.x;
    if(r.y < 0) r.y = -r.y;
    return r;
}

void cvSupport::imageBrowser(vector<string> &images){
    for(uint i = 0; i < images.size(); ++i){
        Mat img = imread(images[i],CV_LOAD_IMAGE_COLOR);
        if(img.data){
            string imgname = Support::getFileName(images[i],true);
            string winname = "Browser: " + Support::getFilePath(images[i]);

            Mat img2show =  cvSupport::addRows(img,20,cvSupport::BORDER_TOP);
            putText(img2show, imgname, Point(0,15), CV_FONT_HERSHEY_PLAIN,1.0,Scalar(255,255,255));
            imshow(winname,img2show);

            indexBrowser(i,images.size());

        }

    }
}

/**
  Keyboard controller for Browser - n = next, b = back, q,ESC to quit
  */
int cvSupport::indexBrowser(uint &index, uint size){


    int event = 0;

    while(event == 0){

        int key = waitKey();
        if(key == 'n'){
            if( index == size-1){
               index--;
            }
            event = 1;
        }
        else if(key == 'b'){
            if(index > 0){
                index--;
                index--;
            }
            else{
                index--;
            }
            event = 1;
        }
        else if(key == 'q' || key == 27){ // 27 == ESC
            index = size;
            event = -1;
        }
//        else{
//            index--;
//            event = 1;
//        }
    }

    return event;
}

Mat cvSupport::addRows(Mat &img, int rows2add, BORDER_PLACE border){

    Mat img_with_border;

    int top, bottom, left, right;
    top = left = bottom = right = 0;

    switch(border){
    case BORDER_TOP:
        top = rows2add;
        break;
    case BORDER_BOTTOM:
        bottom = rows2add;
        break;
    case BORDER_LEFT:
        left = rows2add;
        break;
    case BORDER_RIGHT:
        right = rows2add;
        break;
    }


    copyMakeBorder(img,img_with_border,
                   top,bottom,left, right,
                   0.1, // type (if 1 -> copy last row image?)
                   Scalar(0,0,0));
    return img_with_border;
}


/**
 * @brief cvSupport::angleBottom calculates angle between the linesegment given by two points and the border of the image (in other words the tilt of the line)
 * @param l (not necessary Left) point of the linesegment
 * @param r (not necessary Right) point of the linesegment
 * @return angle in degrees between the linesegment and bottom line of the image
 */
double cvSupport::angleBottom(Point l, Point r){


    // points for the line
    vector<Point> doublePoint;
    doublePoint.push_back(l);
    doublePoint.push_back(r);

    // declaring the output
    Vec4f out;

    fitLine(doublePoint,
            out,
            CV_DIST_L2, // distance type
            0, // 0 optimal value of Numerical parameter C
            0.1, // reps - for accuracy 0.1
            0.1); // aeps - for accuracy 0.1 are good default values


    // collinear vector with the image borderline
    Vec4f V1(1,0,0,0);

    double tilt = acos(V1.dot(out))*180/CV_PI;

    int tiltsign = 1;
    if(out[3] > out[3]+out[1]){
        tiltsign = -1;
    }


    return tiltsign*tilt;

}
cv::Mat cvSupport::copyFromImage(Mat &img, vector<Point> &points){
    // create mask
    Mat mask = Mat::zeros(img.size(), CV_8UC1);
    // fill mask with white
    fillConvexPoly(mask,points,cv::Scalar(255));
    // Extract region using mask for region
    Mat imageROI;
    img.copyTo(imageROI, mask); // 'image' is the image you used to compute the contours.
    // Mat maskROI = mask(roi); // Save this if you want a mask for pixels within the contour in contourRegion.
    return imageROI;
}
