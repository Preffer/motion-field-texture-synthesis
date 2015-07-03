#define _USE_MATH_DEFINES
#include <iostream>
#include <list>
#include <boost/format.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace boost;
using namespace cv;

const int WIDTH = 840;
const int HEIGHT = 480;

const float MIN_SPEED = 0.2;

const float X_POS_BASE = HEIGHT / 2;
const float Y_POS_BASE = WIDTH - 1;

const float X_POS_VARY = HEIGHT;
const float Y_POS_VARY = 0;

const float X_MOTION_BASE = 128;
const float Y_MOTION_BASE = 128;

const float X_MOTION_WEIGHT = 0.02;
const float Y_MOTION_WEIGHT = 0.02;

const int X_FORCE = 240;
const int Y_FORCE = 420;
const int FORCE_RANGE = 108;

const int BORN_SPEED = 10;

const int TTL_BASE = 800;
const int TTL_VARY = 100;
const int TTL_MAX = TTL_BASE + TTL_VARY;

class Particle {
public:
	Particle();
	float xPos;
	float yPos;
	float counter;
	float xSpeed;
	float ySpeed;
	//int ttl;
};

Particle::Particle() {
	xPos = X_POS_BASE + (float(rand()) / RAND_MAX - 0.5) * X_POS_VARY;
	yPos = Y_POS_BASE + (float(rand()) / RAND_MAX - 0.5) * Y_POS_VARY;
	counter = 0;
	xSpeed = 0;
	ySpeed = 0;
	//ttl  = TTL_BASE   + rand() * TTL_VARY / RAND_MAX;

}

int main(int argc, char* argv[]) {
	Mat motion = imread("plast.bmp", CV_LOAD_IMAGE_COLOR);
	resize(motion, motion, Size(WIDTH, HEIGHT));
	//normalize(motion, motion, 0, 255, NORM_MINMAX);

	//imshow("Motion", motion);

	VideoWriter videoOut(
		"fluid-last1.mpg",
		CV_FOURCC('M', 'J', 'P', 'G'),
		30,
		Size(WIDTH, HEIGHT)
		);
	if (!videoOut.isOpened()){
		cerr << "Failed to open " << endl;
		return EXIT_FAILURE;
	}

	list<Particle> particles;
	Mat scene;

	while (true) {
		for (int i = 0; i < BORN_SPEED; i++) {
			particles.push_back(Particle());
		}

		// apply motion
		scene = Mat(Size(WIDTH, HEIGHT), CV_8UC3, Scalar(255, 255, 255));

		for (auto p = particles.begin(); p != particles.end(); p++) {
			int xPosI = int(p->xPos);
			int yPosI = int(p->yPos);
			if (xPosI <= 0 || xPosI >= HEIGHT || yPosI <= 0 || yPosI >= WIDTH) {
				auto ptr = p;
				if ((++ptr) == particles.end()) {
					particles.erase(p);
					break;
				}
				particles.erase(p--);
				continue;
			}
			Vec3b bgrPixel = motion.at<Vec3b>(xPosI, yPosI);
			//cout << bgrPixel << endl;

			float xSpeed =  0;
			float ySpeed = -1;
			float xVector = p->xPos - X_FORCE;
			float yVector = p->yPos - Y_FORCE;
			float dis = powf(xVector, 4) + powf(yVector, 4);
			float flag = xVector > 0 ? -1 : 1;
			
			if (yVector > 0) {
				xSpeed += (1000000 / dis - 0.001 / powf(xVector, 2)) * -yVector * flag;
			} else {
				xSpeed += (1000000 / dis) * -yVector * flag;
			}

			//p->xSpeed = p->xSpeed / 1.1 + (float(bgrPixel[2]) - X_MOTION_BASE) * X_MOTION_WEIGHT;
			//p->ySpeed = p->ySpeed / 1.1 + (float(bgrPixel[1]) - Y_MOTION_BASE) * Y_MOTION_WEIGHT;
			//p->counter = 0;
			xSpeed += p->xSpeed / 1.2 + (float(bgrPixel[2]) - X_MOTION_BASE) * X_MOTION_WEIGHT;
			ySpeed += p->ySpeed / 1.2 + (float(bgrPixel[1]) - Y_MOTION_BASE) * Y_MOTION_WEIGHT;
			

			float length = sqrtf(powf(xSpeed, 2) + powf(ySpeed, 2));
			if (abs(p->xPos - X_FORCE) < MIN_SPEED) {
				continue;
			} else {
				xSpeed /= length;
				ySpeed /= length;
			}
			if (length < 0.5) {
				xSpeed = 0;
				ySpeed = -1;
			}
			p->xPos += xSpeed;
			p->yPos += ySpeed;

			// erase if die
			if (p->xPos < 0 || p->xPos >= HEIGHT || p->yPos < 0 || p->yPos >= WIDTH) {
				particles.erase(p--);
			} else {
				//float alpha = float(p->ttl) / TTL_MAX;
				arrowedLine(scene, Point(p->yPos, p->xPos), Point(p->yPos + 10 * ySpeed, p->xPos + 8 * xSpeed), Scalar(0, 0, 0), 1, 8, 0, 0.1);
			}

		}
		circle(scene, Point(Y_FORCE, X_FORCE), FORCE_RANGE, Scalar(0, 0, 175), 2);
		imshow("Scene", scene);
		//videoOut << scene;
		waitKey(1);
	}

	return EXIT_SUCCESS;
}