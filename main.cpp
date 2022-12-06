#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <iostream>

#include "cfg.h"

using namespace std;
using namespace cv;

// GLOBAL VARIABLE
VideoCapture cap;

String face_cascade_name;
String eye_cascade_name;

CascadeClassifier face_cascade;
CascadeClassifier eye_cascade;

Mat player_focus(Size(DEFAULT_WIDTH, DEFAULT_HEIGHT), CV_8UC3);
Mat game_frame;

int eye_count = 0;

int PLAYER::open_cascade() {

    face_cascade_name = CASCADE_FACE;
    eye_cascade_name = CASCADE_EYE;

    if (!face_cascade.load(face_cascade_name)) { cout << "Error occured during loading face cascade" << endl; return -1; };
    if (!eye_cascade.load(eye_cascade_name)) { cout << "Error occured during loading face cascade" << endl; return -1; };

    cout << "Cascade load successed." << endl;
}

int PLAYER::open_camera() {

    cap.open(0);

    if (!cap.isOpened()) { cout << "CAM OPEN FAILED" << endl; return -1; }

    cap.set(CAP_PROP_FRAME_WIDTH, DEFAULT_WIDTH);
    cap.set(CAP_PROP_FRAME_HEIGHT, DEFAULT_HEIGHT);

    cout << "Camera opened successfully." << endl;
}

void PLAYER::lamping_time() {

    for (int i = 0; i < LAMPING_TIME; i++) {
        cap >> player_focus;
    }
}

double dis_btw_eye(cv::Point& LEFT, cv::Point& RIGHT) {
    double xdiff = LEFT.x - RIGHT.x;
    double ydiff = LEFT.y = RIGHT.y;
    return sqrt((xdiff * xdiff) + (ydiff * ydiff));
}

int GAME::GAME_INIT() {
    game_frame = imread(GAME_IMG, IMREAD_COLOR);
    if (game_frame.empty()) { cout << "Game image loading Error occured." << endl; return -1; }
}

void detect_Eyes(Mat& PLAYER_FOCUS)
{
    // initializing
    Mat grayscale;  // FRAME -> GRAYSCALE
    Point left_eye_coordinate, right_eye_coordinate;    // eye coordinates
    Point focus;

    flip(PLAYER_FOCUS, PLAYER_FOCUS, 1);  // reverse left, right
    cvtColor(PLAYER_FOCUS, grayscale, COLOR_BGR2GRAY); // convert BGR -> GRAY
    equalizeHist(grayscale, grayscale); // enhance image contrast 
    
    int eye_count = 0;      // eye ditected count
    double dis_eye = 0.0;   // distance between left and right eyes;
    // face Detect
    vector<cv::Rect> faces;
    face_cascade.detectMultiScale(grayscale, faces, 1.1, 2, 0 | cv::CASCADE_FIND_BIGGEST_OBJECT, cv::Size(30, 30)); // detect faces

    for (Rect face : faces) {
        Point full_face_coordinate = Point(face.x, face.y); // face ROI start coordinate

        Rect full_faceROI = face;   // Full face ROI
        Mat full_face = grayscale(full_faceROI); // crop full face: Rect -> Mat

        Rect half_faceROI(Point(0, 0), Size(full_face.rows, full_face.cols / 2));   // Half of face ROI
        Mat half_face = full_face(half_faceROI); // crop half of face

        int width = half_face.cols;     // Half face width size
        int height = half_face.rows;    // Half face height size

        Rect left_eyeROI(Point(0, 0), Size(width / 2, height)); // Left Eye ROI
        Rect right_eyeROI(Point(width / 2, 0), Size(width / 2, height));    // Right Eye ROI
        Mat left_eyeArea = half_face(left_eyeROI);  // Left Eye Rect -> Mat
        Mat right_eyeArea = half_face(right_eyeROI);    // Right Eye Rect -> Mat

        vector<cv::Rect> leyes, reyes;
        eye_cascade.detectMultiScale(left_eyeArea, leyes, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, Size(20, 20));   // left eye detect
        eye_cascade.detectMultiScale(right_eyeArea, reyes, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, Size(20, 20));  // right eye detect

        eye_count = 0;
        Point lefteye_center, righteye_center;  // Left eye and Right eye coordinate from Full Face
        if (leyes.size() == 1 && reyes.size() == 1) {    // If 1 eye detected 
            lefteye_center = Point(leyes[0].x + leyes[0].width / 2, leyes[0].y + leyes[0].height / 2);  // left eye center coordinate in left_eye ROI
            left_eye_coordinate = Point(full_face_coordinate.x + lefteye_center.x, full_face_coordinate.y + lefteye_center.y);  // left eye center coordinate in frame
            righteye_center = Point(reyes[0].x + reyes[0].width / 2, reyes[0].y + reyes[0].height / 2);  // right eye center coordinate in right_eye ROI
            right_eye_coordinate = Point(full_face_coordinate.x + righteye_center.x + width / 2, full_face_coordinate.y + righteye_center.y);    // right eye center coordinate in frame
            eye_count = 2;
        }
        else {
            continue;
        }

        if (eye_count == 2) {
                dis_eye =  dis_btw_eye(left_eye_coordinate, right_eye_coordinate);
                circle(PLAYER_FOCUS, left_eye_coordinate, leyes[0].width / 8, EYE_COLOR, 2, -1);   // Circle left eye center 
                circle(PLAYER_FOCUS, right_eye_coordinate, reyes[0].width / 8, EYE_COLOR, 2, -1);   // CIrcle right eye center
        }
        focus = Point(right_eye_coordinate.x - ((right_eye_coordinate.x - left_eye_coordinate.x) / 2), left_eye_coordinate.y - ((right_eye_coordinate.y - left_eye_coordinate.y) / 2));
        circle(PLAYER_FOCUS, focus, 1, FOCUS_COLOR, 4, -1);
    }
}


int main(int argc, char** argv)
{
    GAME game;
    game.GAME_INIT();

    PLAYER player;
    player.open_cascade();
    player.open_camera();
    player.lamping_time();

    while (true)
    {
        cap >> player_focus; // capture 
        if (!player_focus.data) break; // if frame does not have data -> break;
        detect_Eyes(player_focus); // detect face and eyes

        cv::imshow("Webcam", game_frame); // display
        if (cv::waitKey(10) == 27) break; 
    }
    return 0;
}


