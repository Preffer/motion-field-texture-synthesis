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

float BORN_BASE_COL = COLS - 1;
float BORN_BASE_ROW = ROWS / 2;

float BORN_VARY_COL = 0;
float BORN_VARY_ROW = ROWS;

float SPEED_COL = -3;
float SPEED_ROW = 0;

float MOTION_BASE_COL = 128;
float MOTION_BASE_ROW = 128;

float MOTION_WEIGHT_COL = 0.02;
float MOTION_WEIGHT_ROW = 0.02;

int FORCE_COL = 420;
int FORCE_ROW = 240;
int FORCE_RANGE = 108;

int BORN_SPEED = 10;
float MIN_SPEED = 0.2f;

int SWIRL_MIN_COL = 100;
int SWIRL_MAX_COL = 400;
int SWIRL_BASE_COL = 400;
int SWIRL_VARY_COL = 0;

int SWIRL_MIN_ROW = 100;
int SWIRL_MAX_ROW = 400;
int SWIRL_BASE_ROW = 250;
int SWIRL_VARY_ROW = 150;

float SWIRL_DIS = 100.0;
float SWIRL_BORN_SPEED = 0.1;
float SWIRL_RAD = 0.01;

class Particle {
public:
	Particle();
	float row;
	float col;
	float rowSpeed;
	float colSpeed;
};

Particle::Particle() {
	col = BORN_BASE_COL + (float(rand()) / RAND_MAX - 0.5) * BORN_VARY_COL;
	row = BORN_BASE_ROW + (float(rand()) / RAND_MAX - 0.5) * BORN_VARY_ROW;
	colSpeed = SPEED_COL;
	rowSpeed = SPEED_ROW;
}

class Swirl {
public:
	Swirl();
	float row;
	float col;
};

Swirl::Swirl() {
	col = SWIRL_BASE_COL + (float(rand()) / RAND_MAX - 0.5) * SWIRL_VARY_COL;
	row = SWIRL_BASE_ROW + (float(rand()) / RAND_MAX - 0.5) * SWIRL_VARY_ROW;
}

int main(int argc, char* argv[]) {
	Mat motion = imread("plast.bmp", CV_LOAD_IMAGE_COLOR);
	resize(motion, motion, Size(COLS, ROWS));

	list<Particle> particles;
	list<Swirl> swirls;

	Mat scene;
	imshow("Scene", motion);
	//createTrackbar("ROW_SWIRL", "Scene", &ROW_SWIRL, ROWS);
	//createTrackbar("COL_SWIRL", "Scene", &COL_SWIRL, COLS);
	//swirls.push_back(Swirl());

	while (true) {
		for (int i = 0; i < BORN_SPEED; i++) {
			particles.push_back(Particle());
		}
		if ((float(rand()) / RAND_MAX) < 0.05) {
			swirls.push_back(Swirl());
		}

		for (auto s = swirls.begin(); s != swirls.end(); s++) {
			if (s->col < 0 || s->col >= COLS || s->row < 0 || s->row >= ROWS) {
				swirls.erase(s--);
			} else {
				s->col += SPEED_COL;
				s->row += SPEED_ROW;
			}
		}

		// apply motion
		scene = Mat(Size(COLS, ROWS), CV_8UC3, CV_RGB(255, 255, 255));

		for (auto p = particles.begin(); p != particles.end(); p++) {
			Vec3b bgrPixel = motion.at<Vec3b>(p->row, p->col);

			if (p->row > SWIRL_MIN_ROW && p->row < SWIRL_MAX_ROW && p->col > SWIRL_MIN_COL && p->col < SWIRL_MAX_COL) {
				float swirlCol = swirls.front().col;
				float swirlRow = swirls.front().row;
				float colVector = p->col - swirlCol;
				float rowVector = p->row - swirlRow;
				float vectorDis = sqrtf(powf(colVector, 2) + powf(rowVector, 2));

				for (auto s : swirls) {
					float _colVector = p->col - s.col;
					float _rowVector = p->row - s.row;
					float _vectorDis = sqrtf(powf(_colVector, 2) + powf(_rowVector, 2));
					if (_vectorDis < vectorDis) {
						swirlCol = s.col;
						swirlRow = s.row;
						colVector = _colVector;
						rowVector = _rowVector;
						vectorDis = _vectorDis;
					}
				}
				float vectorAngle = atan2f(rowVector, colVector);

				//cout << boost::format("%1%: %2%") % vectorDis % vectorAngle << endl;
				float swirlAngle = 1e15 / powf(vectorDis, 8) * SWIRL_RAD;
				if (swirlAngle > 0.05) {
					swirlAngle = 0.05;
				}
				float newAngle = vectorAngle + swirlAngle;
				//cout << newAngle << endl;
				p->col = swirlCol + vectorDis * cosf(newAngle) + SPEED_COL;
				p->row = swirlRow + vectorDis * sinf(newAngle) + SPEED_ROW;

				if (abs(SWIRL_DIS / vectorDis * cosf(newAngle + M_PI_2)) > abs(SPEED_COL)) {
					p->colSpeed = SWIRL_DIS / vectorDis * cosf(newAngle + M_PI_2);
				} else {
					p->colSpeed = SPEED_COL;
				}

				if (abs(SWIRL_DIS / vectorDis * sinf(newAngle + M_PI_2)) > abs(SPEED_ROW)) {
					p->rowSpeed = SWIRL_DIS / vectorDis * sinf(newAngle + M_PI_2);
				} else {
					p->rowSpeed = SPEED_ROW;
				}

				float speedLength = sqrtf(powf(p->colSpeed, 2) + powf(p->rowSpeed, 2));
				p->colSpeed /= speedLength;
				p->rowSpeed /= speedLength;
				//float speedDis = sqrtf(powf(p->colSpeed, 2) + powf(p->rowSpeed, 2));
				//float speedAngle = atan2f(p->rowSpeed, p->colSpeed) - SWIRL_RAD;
				//cout << speedAngle << endl;

				//float swirlWeight = SWIRL_DIS / vectorDis;
				//cout << swirlWeight << endl;

				//float newAngle = vectorAngle * swirlWeight + speedAngle * (1 - swirlWeight);
				//float newAngle = vectorAngle;
				//cout << boost::format("(%1%, %2%): %3%, after transfer %4%, cos %5%, sin %6%") % p->rowSpeed % p->colSpeed % angle % (angle - SWIRL_RAD) % cos(angle - SWIRL_RAD) % sin(angle - SWIRL_RAD)<< endl;
				//p->colSpeed = speedDis * cosf(newAngle);
				//p->rowSpeed = speedDis * sinf(newAngle);

			} else {
				/*p->colSpeed = (SPEED_COL + p->colSpeed * 9) / 10;
				p->rowSpeed = (SPEED_ROW + p->rowSpeed * 9) / 10;
				p->col += p->colSpeed;
				p->row += p->rowSpeed;*/
				p->col += SPEED_COL;
				p->row += SPEED_ROW;
				p->colSpeed = SPEED_COL / 3;
				p->rowSpeed = SPEED_ROW / 3;
			}

			// erase if die
			if (p->col < 0 || p->col >= COLS || p->row < 0 || p->row >= ROWS) {
				particles.erase(p--);
			} else {
				arrowedLine(scene, Point(p->col, p->row), Point(p->col + 10 * p->colSpeed, p->row + 10 * p->rowSpeed), CV_RGB(0, 0, 0), 1, 8, 0, 0.2);
			}

		}

		for (auto s : swirls) {
			circle(scene, Point(s.col, s.row), 10, CV_RGB(175, 0, 0), 2);
		}
		
		//circle(scene, Point(FORCE_COL, FORCE_ROW), FORCE_RANGE, CV_RGB(175, 0, 0), 2);
		imshow("Scene", scene);
		waitKey(1);
	}

	return EXIT_SUCCESS;
}
