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

float SPEED_COL = -2;
float SPEED_ROW = 0;

float MOTION_BASE_COL = 128;
float MOTION_BASE_ROW = 128;

float MOTION_WEIGHT_COL = 0.02;
float MOTION_WEIGHT_ROW = 0.02;

int FORCE_COL = 420;
int FORCE_ROW = 240;
int FORCE_RANGE = 108;

int BORN_SPEED = 20;
float MIN_SPEED = 0.2f;

int SWIRL_TOP_COL = 0;
int SWIRL_TOP_ROW = 100;

int SWIRL_RIGHT_COL = 240;
int SWIRL_RIGHT_ROW = 240;

int SWIRL_BOTTOM_COL = ROWS - 1;
int SWIRL_BOTTOM_ROW = 100;

int SWIRL_BASE_COL = 240;
int SWIRL_BASE_ROW = 400;

int SWIRL_VARY_COL = 60;
int SWIRL_VARY_ROW = 40;

float SWIRL_LINE1_K = 1;
float SWIRL_LINE1_B = -100;

float SWIRL_LINE2_K = -1;
float SWIRL_LINE2_B = 580;

float SWIRL_DIS = 100.0;
float SWIRL_BORN_INTERVAL = 24;
float SWIRL_RAD = 0.001;

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
	Swirl(float row, float col);
	float row;
	float col;
};

Swirl::Swirl(float row, float col) : row(row), col(col) {}

int main(int argc, char* argv[]) {
	Mat motion = imread("plast.bmp", CV_LOAD_IMAGE_COLOR);
	resize(motion, motion, Size(COLS, ROWS));

	list<Particle> particles;
	list<Swirl> swirls;

	Mat scene;
	imshow("Scene", motion);
	//createTrackbar("ROW_SWIRL", "Scene", &ROW_SWIRL, ROWS);
	//createTrackbar("COL_SWIRL", "Scene", &COL_SWIRL, COLS);

	int frame = 0;
	while (true) {
		for (int i = 0; i < BORN_SPEED; i++) {
			particles.push_back(Particle());
		}
		if (frame++ == 0) {
			for (int i = -4; i < 8; i++) {
				swirls.push_back(Swirl(SWIRL_BASE_COL + i * SWIRL_VARY_COL, SWIRL_BASE_ROW + i * SWIRL_VARY_ROW));
			}
		} else {
			if (frame == SWIRL_BORN_INTERVAL) {
				frame = 0;
			}
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
			float colSpeed = -1;
			float rowSpeed = 0;
			float colVector = p->col - FORCE_COL;
			float rowVector = p->row - FORCE_ROW;
			float dis = powf(colVector, 4) + powf(rowVector	, 4);
			float flag = rowVector > 0 ? -1 : 1;

			if (rowVector > 0) {
				rowSpeed += (3e6 / dis - 1e-8 / powf(rowVector, 2)) * -colVector * flag;
			} else {
				rowSpeed += (3e6 / dis) * -colVector * flag;
			}
			
			p->col += colSpeed;
			p->row += rowSpeed;

			if ((p->col * SWIRL_LINE1_K + SWIRL_LINE1_B < p->row) && (p->col * SWIRL_LINE2_K + SWIRL_LINE2_B > p->row)) {
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
				float swirlAngle = min(1e8 / powf(vectorDis, 4) * SWIRL_RAD, 0.05);
				if (swirlAngle > 0.05) {
					swirlAngle = 0.05;
				}
				float newAngle = vectorAngle + swirlAngle;
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

				p->colSpeed += colSpeed;
				p->rowSpeed += rowSpeed;
				float speedLength = sqrtf(powf(p->colSpeed, 2) + powf(p->rowSpeed, 2));
				p->colSpeed /= speedLength;
				p->rowSpeed /= speedLength;
			} else {
				p->col += SPEED_COL;
				p->row += SPEED_ROW;
				p->colSpeed = SPEED_COL / 3;
				p->rowSpeed = SPEED_ROW / 3;
				p->colSpeed += colSpeed;
				p->rowSpeed += rowSpeed;
			}

			// erase if die
			if (p->col < 0 || p->col >= COLS || p->row < 0 || p->row >= ROWS) {
				particles.erase(p--);
			} else {
				arrowedLine(scene, Point(p->col, p->row), Point(p->col + 8 * p->colSpeed, p->row + 8 * p->rowSpeed), CV_RGB(0, 0, 0), 1, 8, 0, 0.2);
			}

		}

		/*for (auto s : swirls) {
			circle(scene, Point(s.col, s.row), 10, CV_RGB(175, 0, 0), 2);
		}*/
		
		circle(scene, Point(FORCE_COL, FORCE_ROW), FORCE_RANGE, CV_RGB(175, 0, 0), 2);
		imshow("Scene", scene);
		waitKey(1);
	}

	return EXIT_SUCCESS;
}
