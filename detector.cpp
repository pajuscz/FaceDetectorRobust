#include "detector.h"

//##############################################################################
//# common Detector
//##############################################################################
Detector::Detector()
{
}

Detector::Detector(Mat &image)
{
    this->setImage(image);
}

Mat Detector::getGrayImg(){
    return this->_img_gray;
}

void Detector::setImage(Mat &image){
    image.copyTo(this->_img);
    image.copyTo(this->_drawing);
    cvtColor(this->_img,this->_img_gray, CV_BGR2GRAY);
    this->findings = 0;
}
cv::Mat Detector::getImg(){
    return this->_img;
}

Mat Detector::getDrawing(){
    return this->_drawing;
}

int Detector::detect(){

}

void Detector::drawResults(Mat &img, vector<Rect> &results){
    if(img.data){
        for(vector<Rect>::iterator it = results.begin(); it != results.end(); ++it){
            rectangle(img,*it,Scalar(0,255,0));
        }
    }

}

void Detector::setFileName(string filename){
    this->_filename = string(filename);
}
string Detector::getFileName(){
    return this->_filename;
}
int Detector::getFindings(){
    return this->findings;
}
void Detector::incFindings(){
    this->findings++;
}

Rect Detector::maxRect(vector<Rect> &rects){
    int maxArea = 0;
    Rect maxRect;
    for(size_t i = 0; i < rects.size(); ++i){
        Rect r = rects[i];
        if(r.area() > maxArea){
            maxRect = Rect(r);
        }
    }

    return maxRect;
}

void Detector::drawResult(Rect result){
    rectangle(this->_drawing,result, Scalar(0,0,255),2);
}

//##############################################################################
//# Face Detector
//##############################################################################

FaceDetector::FaceDetector(){
    // Load haars from directory
    this->_haars = Support::pathVector("./haars/face",".xml");
    // Init all face cascades;

    for(uint i = 0; i < this->_haars.size() ; ++i){
        CascadeClassifier faceClassifier;

        // couldn't load
        if(!faceClassifier.load(this->_haars[i])){
            cerr << "Can not load face classifier: " << this->_haars[i] << endl;
        }
        else{
            this->FaceClassifiers.push_back(faceClassifier);
        }
    }
    cout << this->FaceClassifiers.size() << " Face classifiers successfully loaded." << endl;
}

cv::Rect FaceDetector::detectFromImage(cv::Mat &image, cv::Mat &rot){
    // add borders for rotating the image
    Mat borderImg = Mat::zeros(800,700,CV_8UC3);
    image.copyTo(borderImg(Rect(80,30,image.cols,image.rows)));


    // set the image
    this->setImage(borderImg);

    // get the image
    cv::Mat mImage = this->getImg();

    // find the best angle
    int bestAngle = rotetedDetector(borderImg); // pass the original borderImage, method won't change it

    // set the original image again
    this->setImage(borderImg);
    mImage = this->getImg();

    cv::Mat rot_mat = cvSupport::rotateImg(mImage, bestAngle, Point(mImage.cols/2, mImage.rows/2));
    this->setImage(mImage);

    this->detectAll();

    // resultedRects from detectAll
    Rect faceRect = this->avgOf2(this->avgRect(this->resultedRects), this->medianRect(this->resultedRects));



    // copy rotation matrix
    rot_mat.copyTo(rot);

    // make cut of the face
    // extend FaceRect

    int height = faceRect.height*0.3;
    int width = faceRect.width*0.1;

    faceRect.height = (faceRect.height + height) > mImage.rows ? mImage.rows : faceRect.height + height;
    faceRect.width = (faceRect.width + width) > mImage.cols ? mImage.cols : faceRect.width + width;
    faceRect.y = (faceRect.y - height) < 0 ? 0 : faceRect.y - height;
    faceRect.x = (faceRect.x - width) < 0 ? 0 : faceRect.x - width;

    mImage(faceRect).copyTo(this->cropped_img);

    //Draw the results
    cv::Mat draw = getDrawing();
    this->drawResult(faceRect);

    return faceRect;
}

int FaceDetector::rotetedDetector(Mat &img){

    cv::Mat tmp_img;
    img.copyTo(tmp_img);

    // Find the best angle
    int bestAngle = 0;
    int results = 0;

    // from -45 to 45, step by 5
    for(int r = -45; r <= 45; r+= 5){
        cvSupport::rotateImg(tmp_img, r, Point(tmp_img.cols/2, tmp_img.rows/2));
        this->setImage(tmp_img);



        int detects = this->detectAll();
        if(detects > results){
            bestAngle = r;
            results = detects;
        }

        // show step by step angle detection
//        Mat draw = getDrawing();
//        this->drawResults(draw,resultedRects);
//        imshow("t",draw);
//        cvWaitKey(0);

        img.copyTo(tmp_img);

    }

    return bestAngle;
}

int FaceDetector::detectAll(){

    Mat image_gray = getGrayImg();

    vector< vector<Rect> > ClassFaces;

    int results = 0;
    this->resultedRects.clear();

    for(uint i = 0; i < FaceClassifiers.size() ; ++i){
        CascadeClassifier fc = FaceClassifiers[i];
        vector<Rect> faces;

        fc.detectMultiScale(image_gray,
                            faces,
                            1.08,
                            0,
                            0|CV_HAAR_DO_ROUGH_SEARCH,
                            Size(150,150)
                            );

        ClassFaces.push_back(faces);


        for( size_t j = 0; j < faces.size(); j++ ){
            cv::Scalar color;
            if(i > cvSupport::N_COLORS){
                color = cvSupport::DEFAULT_COLOR;
            }
            else{
                color = cvSupport::COLORS[i];
            }

            //rectangle(img, faces[j], color, 2);
        }
        //cout << "detected: " << faces.size() << " " << cvSupport::COLORS_NAMES[i]<< " " << this->_haars[i] << endl;

        results += faces.size();

        for(int classes = 0; classes < ClassFaces.size(); classes++ ){


            vector<Rect> fces = ClassFaces[classes];

            Point tl(0,0);
            Point br(0,0);
            int f = 0;

            if(fces.size() > 0){
                for(f = 0; f < fces.size(); f++){

                    tl.x += fces[f].tl().x;
                    tl.y += fces[f].tl().y;

                    br.x += fces[f].br().x;
                    br.y += fces[f].br().y;
                }
                tl.x = tl.x / f;
                tl.y = tl.y / f;
                br.x = br.x / f;
                br.y = br.y / f;

                Rect avg = Rect(tl,br);
                resultedRects.push_back(avg);

            }
        }
    };

    return results;

}


void FaceDetector::saveResult(vector<Rect> & faces, int angle){

    //biggest face
    int MaxArea = 0;
    Rect MaxRect;
    for(int i = 0 ; i < faces.size(); ++i){
        Rect face = faces[i];
        if(face.area() > MaxArea){
            MaxRect = face;
        }
    }

    int tlx = 0;
    int brx = 0;
    int tly = 0;
    int bry = 0;
    //average face
    for(int i = 0 ; i < faces.size(); ++i){
        Rect face = faces[i];
        tlx += face.tl().x;
        brx += face.br().x;
        tly += face.tl().y;
        bry += face.br().y;
    }
    Point tl = Point(tlx/faces.size(), tly/faces.size());
    Point br = Point(brx/faces.size(), bry/faces.size());
    Rect avgFace(tl,br);

    Mat img;
    getDrawing().copyTo(img);
    Mat imgOut(img(MaxRect));
    rotateWithOffset(img,angle);
    //rectangle(img,MaxRect,Scalar(0,255,0),2);
    cout << "saving: " << getFileName() << endl;
    imwrite("result/" + getFileName(),imgOut);
}

void FaceDetector::rotateWithOffset(Mat &img, int angle){
    //! No need to translate before - it is not that benefitial
//    int offsety = -abs(angle)*2*sqrt(2);
//    cvSupport::translateImg(img,0,offsety);
    cvSupport::rotateImg(img,angle,Point(img.cols/2,img.rows * 0.7));
}

void FaceDetector::detection(){
    // Load grayScale Image
    Mat img = getGrayImg();

    vector<Rect> faces;
    // perform the first detection
    int oldResult = detectface(img,faces); // detect from first images


    Mat d = getDrawing();
    //drawResults(d,faces);


    /**
      Experimental Thresholds
    */
    int low_number = 10;
    int high_number = 25;


    // better than hight threshold - result is OK
    if( faces.size() > high_number){
    }
    else if (faces.size() < low_number) {
        int diff;
        int angle = 40;
        //int newResult = detect2Rotate(faces,&angle,&diff);
        cout << "diff: " << diff << endl;
        //if( newResult > oldResult && diff > low_number){
        //    rotateResult(angle,faces);
        //}

    }
    else{ // is in between low && high
        //detect2Rotate(faces,5);

    }

    cout << "ResultRotat: " << faces.size() << endl;

}

void FaceDetector::rotateResult(int angle, vector<Rect> faces){
    Mat draw = getDrawing();

    rotateWithOffset(draw,angle);
    drawResults(draw,faces);
}



int FaceDetector::detectface(Mat &img, vector<Rect> &faces){

    // erase faces
    faces.clear();

    Mat mImgGray;
    // 1. equalize hist - for better performance
    equalizeHist(img, mImgGray);


    incFindings(); // findings++

    // 2. Load cascade
    CascadeClassifier fc;
    fc.load(this->_haars.front());
    // 3. Detect faces
    fc.detectMultiScale(mImgGray,
                        faces,
                        1.05,
                        0,
                        0|CV_HAAR_DO_ROUGH_SEARCH,
                        approxMinSize()
                        );

    // 4. show image
//    Mat drawing = getDrawing();
//    for(size_t i; i < faces.size(); ++i){
//        rectangle(drawing,faces[i],Scalar(0,0,255));
//    }

    return faces.size();
}



Size FaceDetector::approxMinSize(){
    Mat img = getGrayImg();

    double face_size_factor = 0.3;
    Size minsize(img.cols*face_size_factor,img.rows*face_size_factor);

    return minsize;
}

bool pointCompare(const cv::Point &a, const cv::Point &b){
    return a.x <= b.x && a.y <= b.y;
}

cv::Rect FaceDetector::avgOf2(Rect a, Rect b){
    Point tl(0,0);
    Point br(0,0);

    tl.x += a.tl().x + b.tl().x;
    tl.y += a.tl().y + b.tl().y;

    br.x += a.br().x + b.br().x;
    br.y += a.br().y + b.br().y;

        tl.x = tl.x / 2;
        tl.y = tl.y / 2;
        br.x = br.x / 2;
        br.y = br.y / 2;

    Rect avgRect = Rect(tl,br);
    return avgRect;
}

cv::Rect FaceDetector::medianRect(vector<Rect> & faces){
    Point tl(0,0);
    Point br(0,0);
    int f = 0;

    if(faces.size() > 0){
        vector<Rect> rects(faces.begin(), faces.end());

        std::sort(rects.begin(), rects.end(),
                  [](const cv::Rect &a, const cv::Rect &b)
                  {
                      return a.tl().x <= b.tl().x && a.tl().y <= b.tl().y ; //or whatever you like
                  });


        return rects[rects.size()/2];
    }

    return Rect (0,0,0,0);
}



cv::Rect FaceDetector::avgRect(vector<Rect> & faces){

    Point tl(0,0);
    Point br(0,0);
    int f = 0;

    if(faces.size() > 0){
        for(f = 0; f < faces.size(); f++){

            tl.x += faces[f].tl().x;
            tl.y += faces[f].tl().y;

            br.x += faces[f].br().x;
            br.y += faces[f].br().y;
        }
        tl.x = tl.x / f;
        tl.y = tl.y / f;
        br.x = br.x / f;
        br.y = br.y / f;

        Rect avgRect = Rect(tl,br);
        return avgRect;
    }
    return Rect (0,0,0,0);
}




int FaceDetector::detect(){

    Mat mImgGray;
    // 1. equalize hist - for better performance
    equalizeHist(getGrayImg(), mImgGray);

    // 2. Detect faces
    // woah dude magic starts here :)

    vector< vector<Rect> > Faces;
    for(uint i = 0; i < FaceClassifiers.size() ; ++i){
        CascadeClassifier fc = FaceClassifiers[i];
        vector<Rect> faces;

        fc.detectMultiScale(mImgGray,
                            faces,
                            1.3,
                            2,
                            0|CV_HAAR_DO_ROUGH_SEARCH,
                            Size(30,30)
                            );



        cout << i << " - " << faces.size() << endl;
        if(faces.size() > 0){
            Faces.push_back(faces);
        }
    }

    for(uint i = 0; i < Faces.size() ; ++i){
        vector<Rect> faces = Faces[i];
        Mat fimage;
        Mat drawing = getDrawing();
        drawing.copyTo(fimage);
        for(uint f = 0; f < faces.size(); ++f){
            rectangle(fimage,faces[i],Scalar(0,0,255));
            cout << faces[i] << endl;
        }

        imshow("classifier " + std::to_string(i), fimage);

    }

}

cv::Mat FaceDetector::getCroppedImg(){
    return this->cropped_img;
}

int FaceDetector::saveCroppedFace(string filename){
    cout << "Saving: " << filename << endl;
    if(imwrite(filename, this->getCroppedImg())) return 1;
    return -1;
}
