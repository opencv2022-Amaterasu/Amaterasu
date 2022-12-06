#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

/*------ CAMERA_SETTINGS ------- */
#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720
/*------ WINDOW_NAME ------ */
#define WHOLE_IMG	"Webcam"
#define FACE_IMG	"FACE"

/*------ CASCADE_DIR -------*/
#define CASCADE_FACE "haarcascade_frontalface_alt2.xml"
#define CASCADE_EYE "haarcascade_eye.xml"

/*------ INITALIZING ------*/
#define LAMPING_TIME 20

/*------  COLOR ------*/
#define EYE_COLOR Scalar(0,0,255)
#define FOCUS_COLOR Scalar(0,0,0)


/*------  GAME ------*/
#define GAME_IMG "GAME_IMAGE.jpg"


class GAME {
public:
	int GAME_INIT();
};

class PLAYER {
public:
	int open_camera();
	int open_cascade();
	void lamping_time();
};

#endif