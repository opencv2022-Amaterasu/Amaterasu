
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <iostream>

#include <Windows.h>

#include "cfg.h"

using namespace std;
using namespace cv;

//Display
int Display_width, Display_height;

// CAMERA
VideoCapture cap;

// CASCADE NAME
String face_cascade_name;
String eye_cascade_name;

// CASCADE CLASSIFIER
CascadeClassifier face_cascade;
CascadeClassifier eye_cascade;


/*----- player -----*/
// FRAMES
Mat player_focus(Size(DEFAULT_WIDTH, DEFAULT_HEIGHT), CV_8UC3);
Mat player_foucus_ex(Size(DEFAULT_WIDTH, DEFAULT_HEIGHT), CV_8UC3);

/*----- grame -----*/
Mat game_frame;

// MOUSE CURSOR
cv::Point CURSOR;
cv::Point CURSOR_ex;

// COUNTS
int eye_count = 0;

// Weidht
double weight;

int GAME::GAME_INIT() {

    /* PLAYERS GAME MONITER SETTINGS c*/
    cout << BORDER_LINE << endl;
    cout << "Put Display Settings." << endl;
    cout << "Width: ";
    cin >> Display_width;
    cout << "Height: ";
    cin >> Display_height;
    cout << BORDER_LINE << endl;
    game_frame = imread(GAME_IMG, IMREAD_COLOR);
    if (game_frame.empty()) { cout << "Game image loading Error occured." << endl; return -1; }
    ANSWER = Rect(ANSWER_POINT1, ANSWER_POINT2);
    game_frame(ANSWER) = 0;

}

int GAME::GAME_PLAY(cv::Point& CURSUR, cv::Point& CURSUR_EX) {
    /* GET FOCUS COORDINATE */
    if (CURSOR.x == 0 && CURSOR.y == 0) {   // CURSOR ==0 -> return
        return -1;
    }
    else if (CURSOR_ex.x == 0 && CURSOR_ex.y == 0) {    // CURSOR-ex == 0 -> reutrn
        return -1;
    }
    else {
        cv::Point diff = CURSOR - CURSOR_ex;
        circle(game_frame, CURSOR - diff*30, 3, CURSOR_COLOR, 2, FILLED);
    }

}


int PLAYER::open_cascade() {

    face_cascade_name = CASCADE_FACE;
    eye_cascade_name = CASCADE_EYE;

    cout << BORDER_LINE << endl;
    if (!face_cascade.load(face_cascade_name)) { cout << "Error occured during loading face cascade" << endl; return -1; };
    if (!eye_cascade.load(eye_cascade_name)) { cout << "Error occured during loading face cascade" << endl; return -1; };

    cout << "Cascade load successed." << endl;
    cout << BORDER_LINE << endl;
}

int PLAYER::open_camera() {

    cap.open(0);

    cout << BORDER_LINE << endl;
    if (!cap.isOpened()) { cout << "CAM OPEN FAILED" << endl; return -1; }

    cap.set(CAP_PROP_FRAME_WIDTH, DEFAULT_WIDTH);
    cap.set(CAP_PROP_FRAME_HEIGHT, DEFAULT_HEIGHT);

    cout << "Camera opened successfully." << endl;
    cout << BORDER_LINE << endl;
}

void PLAYER::lamping_time() {

    for (int i = 0; i < LAMPING_TIME; i++) {
        cap >> player_focus;
    }
    cout << "Lamping TIME " << endl << BORDER_LINE << endl;
}

void PLAYER::detect_Eyes(cv::Mat& PLAYER_FOCUS, cv::Mat& GAME_FRAME)
{
    // initializing
    Mat grayscale;  // FRAME -> GRAYSCALE
    Point left_eye_coordinate, right_eye_coordinate;    // eye coordinates
    Point focus;

    flip(PLAYER_FOCUS, PLAYER_FOCUS, 1);  // reverse left, right
    cvtColor(PLAYER_FOCUS, grayscale, COLOR_BGR2GRAY); // convert BGR -> GRAY
    equalizeHist(grayscale, grayscale); // enhance image contrast 

    int eye_count = 0;      // eye ditected count
    // face Detect
    vector<cv::Rect> faces;
    face_cascade.detectMultiScale(grayscale, faces, 1.1, 2, 0 | cv::CASCADE_FIND_BIGGEST_OBJECT, cv::Size(30, 30)); // detect faces
    CURSOR_ex = cv::Point(CURSOR);
    if (faces.size() == 0) {

    }
    else if(faces.size() == 1) {
        Rect face = faces[0];
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

            weight = cv::norm(right_eye_coordinate - left_eye_coordinate);
            circle(PLAYER_FOCUS, left_eye_coordinate, leyes[0].width / 8, EYE_COLOR, 2, -1);   // Circle left eye center 
            circle(PLAYER_FOCUS, right_eye_coordinate, reyes[0].width / 8, EYE_COLOR, 2, -1);   // CIrcle right eye center
        }
        focus = Point(right_eye_coordinate.x - ((right_eye_coordinate.x - left_eye_coordinate.x) / 2), left_eye_coordinate.y - ((right_eye_coordinate.y - left_eye_coordinate.y) / 2));
        CURSOR = focus;
        cout << "WEIGHT = " << weight << endl;
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
        player.detect_Eyes(player_focus, game_frame); // detect face and eyes
        game.GAME_PLAY(CURSOR, CURSOR_ex);


        cv::namedWindow(GAME_IMG);
        //cv::namedWindow(WEBCAM_IMG);

        cv::moveWindow(GAME_IMG, (Display_width / 2 - DEFAULT_WIDTH / 2) / 6, (Display_height / 2 - DEFAULT_HEIGHT / 2) / 9);
        //::moveWindow(WEBCAM_IMG, (Display_width / 2 - DEFAULT_WIDTH / 2) / 6, (Display_height / 2 - DEFAULT_HEIGHT / 2) / 9);
        cv::imshow(GAME_IMG, game_frame); // display
        if (cv::waitKey(10) == 27) break;
    }

    destroyAllWindows();
    return 0;
}
