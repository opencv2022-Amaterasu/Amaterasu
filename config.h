#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__
#include <opencv2/opencv.hpp>
//------ FOR MJ ------//
#ifdef _WIN32
#include <Windows.h>
#define sleep(x) Sleep((x*1000))
///------ CAMERA SETTING ------///
#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720

///------ CASCADE DIR ------///
#define CASCADE_FACE "haarcascade_frontalface_alt2.xml"
#define CASCADE_EYE "haarcascade_eye.xml"

#endif

/*
//------ FOR JW ------//
#ifdef __APPLE__
#include <unistd.h>
///------ CAMERA SETTING ------///
#define DEFAULT_WIDTH 3840
#define DEFAULT_HEIGHT 2160

///------ CASCADE DIR ------///
#define CASCADE_FACE "./cascade_xmls/haarcascade_frontalface_alt2.xml"
#define CASCADE_EYE "./cascade_xmls/haarcascade_eye.xml"

#endif
*/

/*------ WINDOW_NAME ------ */
#define GAME_IMG	"GAME"
#define WEBCAM_IMG	"Webcam"
#define FACE_IMG	"FACE"

/*------ INITALIZING ------*/
#define LAMPING_TIME 20
#define CASCADE_FACE_STORAGE_SIZE 30
#define CASCADE_FACE_ERROR_COUNT 30
#define CASCADE_FACE_MIN_NEIGHBORS 9
#define CASCADE_EYES_MIN_NEIGHBORS 15
#define CASCADE_LEFT_EYE_ERROR_COUNT 30
#define CASCADE_RIGHT_EYE_ERROR_COUNT 30

/*------  COLOR ------*/
#define EYE_COLOR cv::Scalar(0,0,255)
#define CURSOR_COLOR cv::Scalar(255, 0 ,255)

/*------  GAME ------*/
#define GAME_IMG_FILE "GAME_IMAGE.jpg"
std::string INIT_TEXT = "Keep watching Display Corners";
/*------ PARAMETERS ------*/
#define EYE_CONTRAST_THRESHOLD 25
#define EYE_CONTRAST_WEIGHT 2.f
#define INITIALIZATION_FRAME_NUM 30

cv::Point ANSWER_POINT1(685, 275);
cv::Point ANSWER_POINT2(715, 335);
cv::Rect ANSWER(ANSWER_POINT1, ANSWER_POINT2);
cv::Mat BOARD(cv::Size(DEFAULT_WIDTH, DEFAULT_HEIGHT), CV_8UC3, cv::Scalar(200,200,200));
cv::Point CENTER_POINT(DEFAULT_WIDTH / 2, DEFAULT_HEIGHT / 2);

class SYS_INITIALIZING {
public:
	int open_cascade();
	int open_camera();
	void lamping_time(cv::Mat& FRAME);
	int initialzing();

};

class GAME {
public:
	int GAME_INIT();
};

class PLAYER {
private:
	std::vector<cv::Point> INITIAL_FOCUS_POINTS; 		// (TL, TR, BR, BL) * 2
	std::vector<double> INITIAL_PERSPECTIVE_WEIGHTS; 	// (TL, TR, BR, BL) * 2
	std::vector<cv::Rect> INITIAL_FACES; 				// (TL, TR, BR, BL) * 2
	std::vector<cv::Rect> INITIAL_LEFT_EYES; 			// (TL, TR, BR, BL) * 2
	std::vector<cv::Rect> INITIAL_RIGHT_EYES; 			// (TL, TR, BR, BL) * 2
	cv::Point focus_point;
	double perspective_weight;
	double min_perspective_weight = 9999.f;
	double max_perspective_weight = 0.f;

public:
	/* Getter & Setter functions */
	void set_focus_point(cv::Point focus) { this->focus_point = focus; }
	cv::Point get_focus_point() { return this->focus_point; }

	void set_perspective_weight(double weight) { this->perspective_weight = weight; }
	double get_perspective_weight() { return this->perspective_weight; }

	void push_to_initial_focus_points(cv::Point focus_point) { this->INITIAL_FOCUS_POINTS.push_back(focus_point); }
	std::vector<cv::Point> get_initial_focus_points() { return this->INITIAL_FOCUS_POINTS; }

	void push_to_initial_perspective_weights(double perspective_weight) { this->INITIAL_PERSPECTIVE_WEIGHTS.push_back(perspective_weight); }
	std::vector<double> get_initial_perspective_weights() { return this->INITIAL_PERSPECTIVE_WEIGHTS; }

	void push_to_initial_faces(cv::Rect face) { this->INITIAL_FACES.push_back(face); }
	std::vector<cv::Rect> get_initial_faces() { return this->INITIAL_FACES; }

	void push_to_initial_left_eyes(cv::Rect left_eye) { this->INITIAL_LEFT_EYES.push_back(left_eye); }
	std::vector<cv::Rect> get_initial_left_eyes() { return this->INITIAL_LEFT_EYES; }

	void push_to_initial_right_eyes(cv::Rect right_eye) { this->INITIAL_RIGHT_EYES.push_back(right_eye); }
	std::vector<cv::Rect> get_initial_right_eyes() { return this->INITIAL_RIGHT_EYES; }

	void initialize_members() { this->set_focus_point(cv::Point()); this->set_perspective_weight(0.); }

	void detect_Eyes(cv::Mat& PLAYER_FOCUS, cv::Mat& GAME_FRAME, cv::Point* EYES_COORDINATE, std::queue<cv::Rect>& DETECTED_FACES_QUEUE, std::queue<cv::Rect>& DETECTED_LEFT_EYE_QUEUE, std::queue<cv::Rect>& DETECTED_RIGHT_EYE_QUEUE);

	void initial_setup(cv::Mat& PLAYER_FOCUS, cv::Mat& GAME_FRAME, cv::Point* EYES_COORDINATE, std::queue<cv::Rect>& DETECTED_FACES_QUEUE, std::queue<cv::Rect>& DETECTED_LEFT_EYE_QUEUE, std::queue<cv::Rect>& DETECTED_RIGHT_EYE_QUEUE);
	
	void show_initial_setups(PLAYER* p);

	cv::Point calulate_focus(PLAYER* p);
};


/*------ etc ------*/
#define BORDER_LINE "------------------------------"

#endif
