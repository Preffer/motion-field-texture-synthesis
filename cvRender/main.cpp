#define _USE_MATH_DEFINES
#include <iostream>
#include <list>
#include <boost/format.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace boost;
using namespace cv;

const int COLS = 840;
const int ROWS = 480;

float COL_BORN_BASE = COLS - 1;
float ROW_BORN_BASE = ROWS / 2;

float COL_BORN_VARY = 0;
float ROW_BORN_VARY = ROWS;

float COL_SPEED = -2;
float ROW_SPEED = 0;

float COL_MOTION_BASE = 128;
float ROW_MOTION_BASE = 128;

float COL_MOTION_WEIGHT = 0.02;
float ROW_MOTION_WEIGHT = 0.02;

int COL_FORCE = 420;
int ROW_FORCE = 240;
int FORCE_RANGE = 108;

int BORN_SPEED = 10;
float MIN_SPEED = 0.2f;

int COL_SWIRL_MIN = 100;
int COL_SWIRL_MAX = 800;
int ROW_SWIRL_MIN = 100;
int ROW_SWIRL_MAX = 400;
int ROW_SWIRL_AVG = (ROW_SWIRL_MIN + ROW_SWIRL_MAX) / 2;
float SWIRL_RAD = 0.05;

class Particle {
public:
	Particle();
	float row;
	float col;
	float rowSpeed;
	float colSpeed;
};

Particle::Particle() {
	col = COL_BORN_BASE + (float(rand()) / RAND_MAX - 0.5) * COL_BORN_VARY;
	row = ROW_BORN_BASE + (float(rand()) / RAND_MAX - 0.5) * ROW_BORN_VARY;
	colSpeed = COL_SPEED;
	rowSpeed = ROW_SPEED;
}

int main(int argc, char* argv[]) {
	Mat motion = imread("plast.bmp", CV_LOAD_IMAGE_COLOR);
	resize(motion, motion, Size(COLS, ROWS));

	list<Particle> particles;
	Mat scene;
	imshow("Scene", motion);
	//createTrackbar("ROW_SWIRL", "Scene", &ROW_SWIRL, ROWS);
	//createTrackbar("COL_SWIRL", "Scene", &COL_SWIRL, COLS);

	while (true) {
		for (int i = 0; i < BORN_SPEED; i++) {
			particles.push_back(Particle());
		}

		// apply motion
		scene = Mat(Size(COLS, ROWS), CV_8UC3, CV_RGB(255, 255, 255));

		for (auto p = particles.begin(); p != particles.end(); p++) {
			Vec3b bgrPixel = motion.at<Vec3b>(p->row, p->col);

			if (p->row > ROW_SWIRL_MIN && p->row < ROW_SWIRL_MAX && p->col > COL_SWIRL_MIN && p->col < COL_SWIRL_MAX) {
				float length = sqrtf(powf(p->colSpeed, 2) + powf(p->rowSpeed, 2));
				if (length == sqrtf(powf(COL_SPEED, 2) + powf(ROW_SPEED, 2))) {
					length *= 3;
				}
				float angle;
				if (p->row < ROW_SWIRL_AVG) {
					angle = atan2f(p->rowSpeed, p->colSpeed) - SWIRL_RAD;
				} else {
					angle = atan2f(p->rowSpeed, p->colSpeed) - SWIRL_RAD;
				}
				//cout << boost::format("(%1%, %2%): %3%, after transfer %4%, cos %5%, sin %6%") % p->rowSpeed % p->colSpeed % angle % (angle - SWIRL_RAD) % cos(angle - SWIRL_RAD) % sin(angle - SWIRL_RAD)<< endl;
				p->colSpeed = length * cosf(angle);
				p->rowSpeed = length * sinf(angle);
				p->col += p->colSpeed + COL_SPEED;
				p->row += p->rowSpeed + ROW_SPEED;
			} else {
				p->colSpeed = (COL_SPEED + p->colSpeed * 4) / 5;
				p->rowSpeed = (ROW_SPEED + p->rowSpeed * 4) / 5;
				p->col += p->colSpeed;
				p->row += p->rowSpeed;
			}

			// erase if die
			if (p->col < 0 || p->col >= COLS || p->row < 0 || p->row >= ROWS) {
				particles.erase(p--);
			} else {
				arrowedLine(scene, Point(p->col, p->row), Point(p->col + 10 * p->colSpeed, p->row + 10 * p->rowSpeed), CV_RGB(0, 0, 0), 1, 8, 0, 0.2);
			}

		}

		circle(scene, Point(COL_FORCE, ROW_FORCE), FORCE_RANGE, CV_RGB(175, 0, 0), 2);
		imshow("Scene", scene);
		waitKey(1);
	}

	return EXIT_SUCCESS;
}
