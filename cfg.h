#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

/*------ CAMERA_SETTINGS ------- */
#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720

/*------ WINDOW_NAME ------ */
#define GAME_IMG	"GAME"
#define WEBCAM_IMG	"Webcam"
#define FACE_IMG	"FACE"

/*------ CASCADE_DIR -------*/
#define CASCADE_FACE "haarcascade_frontalface_alt2.xml"
#define CASCADE_EYE "haarcascade_eye.xml"

/*------ INITALIZING ------*/
#define LAMPING_TIME 20
#define FACE_STORAGE_SIZE 30

/*------  COLOR ------*/
#define EYE_COLOR Scalar(0,0,255)
#define CURSOR_COLOR Scalar(0, 0,255)

/*------  GAME ------*/
#define GAME_IMG "GAME_IMAGE.jpg"

cv::Point ANSWER_POINT1(685, 275);
cv::Point ANSWER_POINT2(715, 335);


class GAME {
	cv::Rect ANSWER;


public:
	int GAME_INIT();
	int GAME_PLAY(cv::Point& CURSUR, cv::Point& CURSUR_EX);
};

class PLAYER {
public:
	int open_camera();
	int open_cascade();
	void lamping_time();
	void detect_Eyes(cv::Mat& PLAYER_FOCUS, cv::Mat& GAME_FRAME);
};

/*------  etc ------*/
#define BORDER_LINE "------------------------------"


#endif
