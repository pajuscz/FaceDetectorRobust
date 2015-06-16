#include "facedescription.h"


DoublePoint::DoublePoint(){

}

DoublePoint::DoublePoint(Point left, Point right){
    this->left = left;
    this->right = right;
}

void DoublePoint::setL_XY(int x, int y){
    this->left = Point(x,y);
}
void DoublePoint::setR_XY(int x, int y){
    this->right = Point(x,y);
}
void DoublePoint::setPointL_XY(Point left){
    this->left = left;
}
void DoublePoint::setPointR_XY(Point right){
    this->right = right;
}

Point DoublePoint::getLPoint(){
    return left;
}
Point DoublePoint::getRPoint(){
    return right;
}

void DoublePoint::rotate(Mat m){
    setPointL_XY(cvSupport::rotatePoint2D(getLPoint(),m));
    setPointR_XY(cvSupport::rotatePoint2D(getRPoint(),m));
}

void DoublePoint::draw(Mat img){
    circle(img,getLPoint(),2,Scalar(0,200,100),2);
    circle(img,getRPoint(),2,Scalar(0,200,100),2);
    circle(img,getCenter(),2,Scalar(200,200,255),1);
}

Point DoublePoint::getCenter(){
    int x_min = (this->left.x < this->right.x) ? this->left.x : this->right.x;
    int y_min = (this->left.y < this->right.y) ? this->left.y : this->right.y;

    Point center = Point(
    x_min+
    abs(this->right.x - this->left.x)/2,
    y_min +
    abs(this->right.y - this->left.y)/2);
    return center;
}

double DoublePoint::distance(){
    return cv::norm(this->left - this->right);
}

Eye::Eye(){
    open = true;
}

Eye::Eye(bool state){
    setOpen(state);
}

Eye::Eye(Point l, Point r){
    setPointL_XY(l);
    setPointR_XY(r);
}

Eye::Eye(Point l, Point r, bool state){
    setPointL_XY(l);
    setPointR_XY(r);
    setOpen(state);
}

string Eye::state(){
    if(open){
        return M_O;
    }
    else{
        return M_C;
    }
}


void Eye::setOpen(bool open){
    this->open = open;
}

bool Eye::getState(){
    return this->open;
}

void Eye::setState(string state){
    if(state == M_O){
        this->open = true;
    }
    else{
        this->open = false;
    }
}


MouthSimple::MouthSimple(){
    this->state = FaceState::DEFAULT;
}

MouthSimple::MouthSimple(Point l, Point r){
    this->setPointL_XY(l);
    this->setPointR_XY(r);
    this->state = FaceState::DEFAULT;
}


void MouthSimple::setState(int statenum){
    this->state = statenum;
}

void MouthSimple::setState(string state){
    for(int i = 0; i < MOUTH_STATES; i++){
        if(state == MOUTH_STATE_STR[i]){
            this->state = i;
            return;
        }
    }
}

int MouthSimple::getState(){
    return this->state;
}

string MouthSimple::getStrState(){
    return MOUTH_STATE_STR[this->state];
}

void MouthSimple::setOpen(bool o){

}

FaceState::FaceState(){

}
FaceState::FaceState(Eye leye, Eye reye, MouthSimple mouth){
    this->lEye = leye;
    this->rEye = reye;
    this->mouth = mouth;
}

void FaceState::openFromFileRaw(string filepath, string imgPath){
    FileStorage fs(filepath,FileStorage::READ);

    Point leyel,leyer,reyel,reyer,nose,mouthl,mouthr;
    string leyes, reyes, mstate, tilt;

    if(fs.isOpened()){
        cout << "Open File: " << filepath << "\tImageFile: " << imgPath << endl;

        // Left eye
        // -------------------------------------------------------------
        fs["LeftEye l"] >> leyel;
        fs["LeftEye r"] >> leyer;
        fs["LeftEye o"] >> leyes;
        int state = (leyes == "OPEN") ? true : false;
        // create LeftEye
        this->lEye = Eye(leyel, leyer, state);

        // Right Eye
        // --------------------------------------------------------------
        fs["RightEye l"] >>  reyel;
        fs["RightEye r"] >>  reyer;
        fs["RightEye o"] >> reyes;
        state = (reyes == "OPEN") ? true : false;
        // Create RightEye
        this->rEye = Eye(reyel, reyer, state);



//        fs["Nose"] >> nose;

        fs["Mouth l"] >> mouthl;
        fs["Mouth r"] >> mouthr;
        fs["Mouth"] >> mstate;
        this->mouth.setState(mstate);
        this->mouth.setPointL_XY(mouthl);
        this->mouth.setPointR_XY(mouthr);


        // calc Tilt
        // ----------------------------------------------------------------
        // prepare vector with eyes
        Vec4f out;
        vector<Point> eyes;
        // Push eye points
        eyes.push_back(this->lEye.getLPoint());
        eyes.push_back(this->lEye.getRPoint());
        eyes.push_back(this->rEye.getLPoint());
        eyes.push_back(this->rEye.getRPoint());
        // fit line from eyes points
        fitLine(eyes, out,CV_DIST_L2,0,0.1,0.1 );

        // sign correction
        int tiltsign = 1;
        if(out[3] > out[3]+out[1]){
            tiltsign = -1;
        }
        // dotproduct with collinear vector of image edge (both vectors size of 1)
        Vec4f V1(1,0,0,0);
        double headtilt = acos(V1.dot(out))*180/CV_PI;
        this->tilt = headtilt * tiltsign;
    }
}

void FaceState::openFromFile(string filepath){
    //init();
    FileStorage fs(filepath,FileStorage::READ);

    Point leyel,leyer,reyel,reyer,nose,mouthl,mouthr;
    string leyes, reyes, mstate, tilt;


    if(fs.isOpened()){

        cout << "Anotation File: " << filepath << endl;


        fs["LeftEye l"] >> leyel;
        fs["LeftEye r"] >> leyer;
        fs["LeftEye o"] >> leyes;

        this->lEye = Eye(leyel,leyer);
        this->lEye.setState(leyes);

        fs["RightEye l"] >>  reyel;
        fs["RightEye r"] >>  reyer;
        fs["RightEye o"] >> reyes;

        fs["Mouth l"] >> mouthl;
        fs["Mouth r"] >> mouthr;
        fs["Mouth"] >> mstate;

        this->rEye = Eye(reyel,reyer);
        this->rEye.setState(reyes);


        this->mouth = MouthSimple(mouthl,mouthr);
        this->mouth.setState(mstate);


        fs["Nose"] >> nose;
        fs["Tilt"] >> tilt;
    }
}

