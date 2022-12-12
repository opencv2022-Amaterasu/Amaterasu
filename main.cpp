#include <opencv2/opencv.hpp>
#include <iostream>
#include "config.h"

using namespace std;
using namespace cv;

// CASCADE CLASSIFIER
CascadeClassifier face_cascade;
CascadeClassifier eye_cascade;

// CAMERA
VideoCapture cap;

/*----- game -----*/
Mat game_frame;
Point focus_ex(0,0);

/*----- player -----*/
Mat player_focus(Size(DEFAULT_WIDTH, DEFAULT_HEIGHT), CV_8UC3);


template<class T>
T get_most_frequent_value(T* buffer) {
    T keys[INITIALIZATION_FRAME_NUM] = { T() };
    int vals[INITIALIZATION_FRAME_NUM] = { 0 };
    int ptr = 0;
    int freqeuncy = 0;
    bool isFound = false;
    T result;

    for (int i = 0; i < INITIALIZATION_FRAME_NUM; i++) {
        int k;
        for (k = 0; k < ptr; k++) {
            if (keys[k] == buffer[i]) {
                isFound = true;
                break;
            }
        }
        if (isFound) {
            // found
            vals[k]++;
            isFound = false;
        }
        else {
            // not found
            keys[ptr] = buffer[i];
            vals[ptr++]++;
        }
    }

    int j;
    for (j = 0; j < INITIALIZATION_FRAME_NUM; j++) {
        if (vals[j] > freqeuncy) {
            freqeuncy = vals[j];
            result = keys[j];
        }
    }

    return result;
}
/************************************************************************************************************************/
/* INITIALIZING                                                                                                         */
/************************************************************************************************************************/
int SYS_INITIALIZING::open_cascade() {
    face_cascade.load(CASCADE_FACE);
    eye_cascade.load(CASCADE_EYE);

    cout << BORDER_LINE << endl;
    if (face_cascade.empty()) { cout << "Error occured during loading face cascade" << endl; return 0; };
    if (eye_cascade.empty()) { cout << "Error occured during loading face cascade" << endl; return 0; };

    cout << "Cascade load successed." << endl;
    cout << BORDER_LINE << endl;
    return 1;
}

int SYS_INITIALIZING::open_camera() {
    cap.open(0);

    cout << BORDER_LINE << endl;
    if (!cap.isOpened()) { cout << "CAM OPEN FAILED" << endl; return 0; }

    cap.set(CAP_PROP_FRAME_WIDTH, DEFAULT_WIDTH);
    cap.set(CAP_PROP_FRAME_HEIGHT, DEFAULT_HEIGHT);

    cout << "Camera opened successfully." << endl;
    cout << BORDER_LINE << endl;
    return 1;
}

void SYS_INITIALIZING::lamping_time(cv::Mat& FRAME) {

    for (int i = 0; i < LAMPING_TIME; i++) {
        cap >> FRAME;
    }
    cout << "Lamping TIME " << endl << BORDER_LINE << endl;
}

int SYS_INITIALIZING::initialzing() {
    cout << BORDER_LINE << endl;
    cout << "GAME IMAGE SIZE: " << DEFAULT_WIDTH << "x" << DEFAULT_HEIGHT << endl;
    return 1;
}
/************************************************************************************************************************/
/* GAME                                                                                                                 */
/************************************************************************************************************************/
int GAME::GAME_INIT() {
    game_frame = imread(GAME_IMG_FILE, IMREAD_COLOR);
    if (game_frame.empty()) { cout << "Game image loading Error occured." << endl; return 0; }
    else {
        cout << "GAME IMGE LOADED." << endl;
        cout << BORDER_LINE << endl;
        ANSWER = Rect(ANSWER_POINT1, ANSWER_POINT2);
        game_frame(ANSWER) = 0; // Masking Answer
        return 1;
    }
}
/************************************************************************************************************************/
/* PLAYER                                                                                                               */
/************************************************************************************************************************/
void PLAYER::detect_Eyes(Mat& PLAYER_FOCUS, Mat& GAME_FRAME, Point* EYES_COORDINATE, queue<Rect>& DETECTED_FACES_QUEUE, queue<Rect>& DETECTED_LEFT_EYE_QUEUE, queue<Rect>& DETECTED_RIGHT_EYE_QUEUE) {
    /*----- INITIALIZATION -----*/
    // FLIPPING & CONVERT2GRAY 
    Mat grayscale;  // PALYER_FOCUS -> GRAYSCALE
    // FACE DETECTION
    vector<Rect> faces;
    Rect face_ROI;
    uint16_t faceErrorCount = 0;
    Point face_ROI_coordinate;
    uint16_t face_width, face_height;
    // SPLIT BOTH EYES ROI
    Rect left_eye_ROI, right_eye_ROI;
    // EYE DETECTION
    vector<Rect> detected_left_eyes, detected_right_eyes;
    Rect left_eye_rect, right_eye_rect;
    uint16_t left_eye_errorCount = 0, right_eye_errorCount = 0;
    //CALCULATE ACCULATE COORDINATE
    Point left_eye_coordinate, right_eye_coordinate;


    //UPDATE VARIABLES
    uint16_t left_eye_x, left_eye_y, right_eye_x, right_eye_y;

    /*----- FLIPPING & CONVERT2GRAY -----*/
    flip(PLAYER_FOCUS, PLAYER_FOCUS, 1);  // reverse left, right
    cvtColor(PLAYER_FOCUS, grayscale, COLOR_BGR2GRAY); // convert BGR -> GRAY

    /*----- FACE DETECTION -----*/
    face_cascade.detectMultiScale(grayscale, faces, 1.1, CASCADE_FACE_MIN_NEIGHBORS);

    // TODO:프레임 간의 얼굴 영역 마진 처리
    if (faces.size() == 1) {            // DETECT SINGLE FACE
        face_ROI = faces.at(0);
        if (DETECTED_FACES_QUEUE.size() > CASCADE_FACE_STORAGE_SIZE) {
            DETECTED_FACES_QUEUE.pop();
        }
        DETECTED_FACES_QUEUE.push(face_ROI);
    }
    else {                          // DETECT MULTIPLE FACES
        /* CASCADE_FACE_ERROR_COUNT만큼 오류 처리 후에도 얼굴이 존재하지 않거나 2개 이상이면 가장 최근 얼굴로 강제 초기화 */
        if (faceErrorCount < CASCADE_FACE_ERROR_COUNT) {
            if (DETECTED_FACES_QUEUE.size() > 1) {
                face_ROI = DETECTED_FACES_QUEUE.back();
                faceErrorCount++;
            }
            else {
                face_ROI = Rect();
            }
        }
        else {
            queue<Rect> eraser;
            swap(eraser, DETECTED_FACES_QUEUE); // Erase entire queue.
            this->initialize_members();
            face_ROI = Rect();
            faceErrorCount = 0;
        }
    }

    // Detected FACE
    rectangle(PLAYER_FOCUS, face_ROI, EYE_COLOR, 2, LINE_AA);

    /*----- SPLIT BOTH EYES ROI -----*/
    face_ROI_coordinate = Point(face_ROI.x, face_ROI.y);
    face_width = cvRound(face_ROI.width);
    face_height = cvRound(face_ROI.height / 2);
    left_eye_ROI = Rect(face_ROI_coordinate.x, face_ROI_coordinate.y, face_width / 2, face_height);
    right_eye_ROI = Rect(face_ROI_coordinate.x + face_width / 2, face_ROI_coordinate.y, face_width / 2, face_height);

    /*----- EYE DETECTION -----*/
    eye_cascade.detectMultiScale(PLAYER_FOCUS(left_eye_ROI), detected_left_eyes, 1.1, CASCADE_EYES_MIN_NEIGHBORS);
    eye_cascade.detectMultiScale(PLAYER_FOCUS(right_eye_ROI), detected_right_eyes, 1.1, CASCADE_EYES_MIN_NEIGHBORS);

    // LEFT EYE
    if (detected_left_eyes.size() == 1) {// DETECT SINGLE LEFT EYE

        left_eye_rect = detected_left_eyes[0];
        if (DETECTED_LEFT_EYE_QUEUE.size() > CASCADE_LEFT_EYE_ERROR_COUNT) {
            DETECTED_LEFT_EYE_QUEUE.pop();
        }
        // DETECTED_LEFT_EYE_QUEUE.push(left_eye_coordinate);
        DETECTED_LEFT_EYE_QUEUE.push(left_eye_rect);
    }
    else {                          // DETECT MULTIPLE LEFT EYES
        /* CASCADE_LEFT_EYE_ERROR_COUNT만큼 오류 처리 후에도 얼굴이 존재하지 않거나 2개 이상이면 가장 최근 얼굴로 강제 초기화 */
        if (left_eye_errorCount < CASCADE_LEFT_EYE_ERROR_COUNT) {
            if (DETECTED_LEFT_EYE_QUEUE.size() > 1) {
                // left_eye_coordinate = DETECTED_LEFT_EYE_QUEUE.back();
                left_eye_rect = DETECTED_LEFT_EYE_QUEUE.back();
                left_eye_errorCount++;
            }
            else {
                // left_eye_coordinate = Point();
                left_eye_rect = Rect();
            }
        }
        else {
            queue<Rect> eraser;
            swap(eraser, DETECTED_LEFT_EYE_QUEUE); // Erase entire queue.
            this->initialize_members();
            // left_eye_coordinate = Point();
            left_eye_rect = Rect();
            left_eye_errorCount = 0;
        }
    }
    // RIGHT EYE
    if (detected_right_eyes.size() == 1) {// DETECT SINGLE RIGHT EYE

        right_eye_rect = detected_right_eyes[0];
        if (DETECTED_RIGHT_EYE_QUEUE.size() > CASCADE_RIGHT_EYE_ERROR_COUNT) {
            DETECTED_RIGHT_EYE_QUEUE.pop();
        }
        // DETECTED_RIGHT_EYE_QUEUE.push(right_eye_coordinate);
        DETECTED_RIGHT_EYE_QUEUE.push(right_eye_rect);
    }
    else {                          // DETECT MULTIPLE RIGHT EYES
        /* CASCADE_RIGHT_EYE_ERROR_COUNT만큼 오류 처리 후에도 얼굴이 존재하지 않거나 2개 이상이면 가장 최근 얼굴로 강제 초기화 */
        if (right_eye_errorCount < CASCADE_RIGHT_EYE_ERROR_COUNT) {
            if (DETECTED_LEFT_EYE_QUEUE.size() > 1) {
                // right_eye_coordinate = DETECTED_RIGHT_EYE_QUEUE.back();
                right_eye_rect = DETECTED_RIGHT_EYE_QUEUE.back();
                right_eye_errorCount++;
            }
            else {
                // right_eye_coordinate = Point();
                right_eye_rect = Rect();
            }
        }
        else {
            queue<Rect> eraser;
            swap(eraser, DETECTED_RIGHT_EYE_QUEUE); // Erase entire queue.
            this->initialize_members();
            // right_eye_coordinate = Point();
            right_eye_rect = Rect();
            right_eye_errorCount = 0;
        }
    }

    left_eye_coordinate = Point((left_eye_rect.x + (left_eye_rect.width / 2)),
        (left_eye_rect.y + (left_eye_rect.height / 2)));
    right_eye_coordinate = Point((right_eye_rect.x + (right_eye_rect.width / 2)),
        (right_eye_rect.y + (right_eye_rect.height / 2)));

    /* UPDATE VARIABLES */
    left_eye_x = left_eye_ROI.x + left_eye_coordinate.x;
    left_eye_y = left_eye_ROI.y + left_eye_coordinate.y;
    right_eye_x = right_eye_ROI.x + right_eye_coordinate.x;
    right_eye_y = right_eye_ROI.y + right_eye_coordinate.y;

    *(EYES_COORDINATE) = Point(left_eye_x, left_eye_y);     // LEFT EYE
    *(EYES_COORDINATE + 1) = Point(right_eye_x, right_eye_y); // RIGHT EYE

    circle(PLAYER_FOCUS, *(EYES_COORDINATE), 3, EYE_COLOR, 1, cv::FILLED);
    circle(PLAYER_FOCUS, *(EYES_COORDINATE + 1), 3, EYE_COLOR, 1, cv::FILLED);

    /* CALCULATE AND UPDATE FOCUS POINT & PERSPECTIVE WEIGHT */
    this->set_focus_point(Point(((right_eye_x - left_eye_x) / 2) + left_eye_x,
        left_eye_y < right_eye_y ? ((left_eye_y - right_eye_y) / 2) + left_eye_y : ((right_eye_y - left_eye_y) / 2) + right_eye_y));
    this->set_perspective_weight(norm(Point(right_eye_x, right_eye_y) - Point(left_eye_x, left_eye_y)));
}

void PLAYER::show_initial_setups(PLAYER* p) {
    // 초기 설정 값 출력 
    cout << endl << BORDER_LINE << "[ INITIAL STATES ]" << endl;
    cout << "Focus Points : ";
    for (int i = 0; i < 8; i++) {
        cout << p->get_initial_focus_points()[i] << "\t";
    }
    cout << endl << endl << "Perspective Weights : ";
    for (int i = 0; i < 8; i++) {
        cout << p->get_initial_perspective_weights()[i] << "\t";
        if (p->get_initial_perspective_weights()[i] > p->max_perspective_weight) {
            p->max_perspective_weight = p->get_initial_perspective_weights()[i];
        }
        else if (p->get_initial_perspective_weights()[i] < p->min_perspective_weight) {
            p->min_perspective_weight = p->get_initial_perspective_weights()[i];
        }
    }
    cout << endl << "min: " << p->min_perspective_weight << endl;
    cout << "max: " << p->max_perspective_weight << endl;

    cout << endl << endl << "Faces : ";
    for (int i = 0; i < 8; i++) {
        cout << p->get_initial_faces()[i] << "\t";
    }
    cout << endl << endl << "Left Eyes : ";
    for (int i = 0; i < 8; i++) {
        cout << p->get_initial_left_eyes()[i] << "\t";
    }
    cout << endl << endl << "Right Eyes : ";
    for (int i = 0; i < 8; i++) {
        cout << p->get_initial_right_eyes()[i] << "\t";
    }
}

void PLAYER::initial_setup(Mat& PLAYER_FOCUS, Mat& GAME_FRAME, Point* EYES_COORDINATE, queue<Rect>& DETECTED_FACES_QUEUE, queue<Rect>& DETECTED_LEFT_EYE_QUEUE, queue<Rect>& DETECTED_RIGHT_EYE_QUEUE) {
    /* VARIABLES */
    Point DISPLAY_TOP_LEFT(0, 0), DISPLAY_TOP_RIGHT(DEFAULT_WIDTH - 1, 0), DISPLAY_BOTTON_RIGHT(DEFAULT_WIDTH - 1, DEFAULT_HEIGHT - 1), DISPLAY_BOTTOM_LEFT(0, DEFAULT_HEIGHT - 1);
    vector<Point> DISPLAY_CORNERS = { DISPLAY_TOP_LEFT, DISPLAY_TOP_RIGHT, DISPLAY_BOTTON_RIGHT, DISPLAY_BOTTOM_LEFT };
    Rect face_buf[INITIALIZATION_FRAME_NUM], left_eye_buf[INITIALIZATION_FRAME_NUM], right_eye_buf[INITIALIZATION_FRAME_NUM];
    Point focus_point_buf[INITIALIZATION_FRAME_NUM];
    double perspective_weight_buf[INITIALIZATION_FRAME_NUM];

    /* INITIALIZATION STEP */
    for (int t = 0; t < 2; t++) {                 // 2번 반복
        // TODO: 추후 PLAYER_FOCUS가 아닌 GAME_FRAME으로 변경 필요
        for (Point corner : DISPLAY_CORNERS) {    // 4 코너
            // TODO: 추후 PLAYER_FOCUS가 아닌 GAME_FRAME으로 변경 필요

            int buf_size = 0;
            while(buf_size < INITIALIZATION_FRAME_NUM) {        // INITIALIZATION_FRAME_NUM: 30 프레임 계산
                cout << "STEP" << buf_size + 1 << endl;
                cap >> PLAYER_FOCUS;
                // 동공 검출 과정

                this->detect_Eyes(PLAYER_FOCUS, GAME_FRAME, EYES_COORDINATE, DETECTED_FACES_QUEUE, DETECTED_LEFT_EYE_QUEUE, DETECTED_RIGHT_EYE_QUEUE);
                
                if (DETECTED_LEFT_EYE_QUEUE.size() != 0 && DETECTED_RIGHT_EYE_QUEUE.size() != 0) {
                    // 검출 결과 버퍼 저장
                    face_buf[buf_size] = DETECTED_FACES_QUEUE.back();
                    cout << "face detected." << endl;
                    left_eye_buf[buf_size] = DETECTED_LEFT_EYE_QUEUE.back();
                    cout << "left eye detected." << endl;
                    right_eye_buf[buf_size] = DETECTED_RIGHT_EYE_QUEUE.back();
                    cout << "right eye detected." << endl;
                    focus_point_buf[buf_size] = this->get_focus_point();
                    cout << "point buff made." << endl;
                    perspective_weight_buf[buf_size] = this->get_perspective_weight();
                    buf_size++;
                    // TODO: 추후 게임 화면에서 표시할 경우 수정 및 삭제 필요
                    putText(PLAYER_FOCUS, INIT_TEXT, Point(10, 30), 2, 1, Scalar(0, 0, 255));
                    circle(PLAYER_FOCUS, corner, 20, Scalar(0, 0, 255), -1, LINE_AA);
                    imshow("Player", PLAYER_FOCUS);
                    waitKey(10);
                }
            }

            // 버퍼 결과로부터 빈도수 최상위 값 계산 및 저장
            this->push_to_initial_faces(get_most_frequent_value(face_buf));
            this->push_to_initial_left_eyes(get_most_frequent_value(left_eye_buf));
            this->push_to_initial_right_eyes(get_most_frequent_value(right_eye_buf));
            this->push_to_initial_focus_points(get_most_frequent_value(focus_point_buf));
            this->push_to_initial_perspective_weights(get_most_frequent_value(perspective_weight_buf));
        }
    }
}


cv::Point PLAYER::calulate_focus(PLAYER* p) {
    double pw = p->get_perspective_weight();
    Point fp = p->get_focus_point();

    if (focus_ex.x == 0 && focus_ex.y == 0) {
        focus_ex = CENTER_POINT;
    }
    
    Point result = (fp - focus_ex) * p->perspective_weight;
    focus_ex = result;

    return result;


}
/************************************************************************************************************************/
/* Main                                                                                                                 */
/************************************************************************************************************************/
int main() {
    /*----- variables -----*/
    queue<Rect> detected_faces_queue, detected_left_eye_queue, detected_right_eye_queue;
    Point eyes_coordinate[2]; //[0]:Left, [1]:Right

    // SYSTEM_INITIALIZING
    SYS_INITIALIZING initializer;
    initializer.initialzing();
    if (!initializer.open_cascade()) { exit(1); }
    if (!initializer.open_camera()) { exit(1); }
    initializer.lamping_time(player_focus);

    GAME game;
    game.GAME_INIT();

    PLAYER player;
    player.initial_setup(player_focus, game_frame, eyes_coordinate, detected_faces_queue, detected_left_eye_queue, detected_right_eye_queue);
    player.show_initial_setups(&player);

    
    while (true) {
        cap >> player_focus;
        player.detect_Eyes(player_focus, game_frame, eyes_coordinate, detected_faces_queue, detected_left_eye_queue, detected_right_eye_queue);
        Point fp = player.calulate_focus(&player);

        // 초점 및 가중치 표현
        circle(player_focus, fp, 3, CURSOR_COLOR, -1, LINE_AA);
        String norm_val = to_string(player.get_perspective_weight());
        putText(player_focus, norm_val, Point(10, 30), 2, 1, Scalar(0, 0, 255));

        // cvtColor(player_focus, player_focus, COLOR_BGR2GRAY);
        imshow("Player", player_focus);
        if (waitKey(10) == 27) { break; }
    }

    destroyAllWindows();

}
