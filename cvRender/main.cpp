#include <iostream>
#include <list>
#include <boost/format.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace boost;
using namespace cv;

const int WIDTH = 512;
const int HEIGHT = 512;

const float X_POS_BASE = 480;
const float Y_POS_BASE = 32;

const float X_POS_VARY = 32;
const float Y_POS_VARY = 32;

const float X_SPEED_BASE = 0;
const float Y_SPEED_BASE = 0;

const float X_SPEED_VARY = -256;
const float Y_SPEED_VARY = 256;

const float X_SPEED_FULL = 4;
const float Y_SPEED_FULL = 4;

const float X_MOTION_BASE = 128;
const float Y_MOTION_BASE = 128;

const float X_MOTION_WEIGHT = 0.02;
const float Y_MOTION_WEIGHT = 0.02;

const int BORN_SPEED = 100;
const int TTL_BASE = 900;
const int TTL_VARY = 100;
const int TTL_MAX = 1000;

class Particle {
public:
	Particle();
	float xPos;
	float yPos;
	float xSpeed;
	float ySpeed;
	int ttl;
};

Particle::Particle() {
	xPos   = X_POS_BASE   + rand() * X_POS_VARY   / RAND_MAX;
	yPos   = Y_POS_BASE   + rand() * Y_POS_VARY   / RAND_MAX;
	xSpeed = X_SPEED_BASE + rand() * X_SPEED_VARY / RAND_MAX;
	ySpeed = Y_SPEED_BASE + rand() * Y_SPEED_VARY / RAND_MAX;
	ttl    = TTL_BASE     + rand() * TTL_VARY     / RAND_MAX;
}

int main(int argc, char* argv[]) {
	Mat motion = imread("motion.bmp", CV_LOAD_IMAGE_COLOR);
	resize(motion, motion, Size(WIDTH, HEIGHT));

	list<Particle> particles;

	while (true) {
		for (int i = 0; i < BORN_SPEED; i++) {
			particles.push_back(Particle());
		}

		// apply motion
		for (auto p = particles.begin(); p != particles.end(); p++) {
			Vec3b bgrPixel = motion.at<Vec3b>(p->xPos, p->yPos);

			float xSpeed = p->xSpeed + (bgrPixel[2] - X_MOTION_BASE) * X_MOTION_WEIGHT;
			float ySpeed = p->xSpeed + (bgrPixel[1] - Y_MOTION_BASE) * Y_MOTION_WEIGHT;
			float length = sqrtf(powf(xSpeed, 2) + powf(ySpeed, 2));

			p->xSpeed = xSpeed / length * X_SPEED_FULL * p->ttl / TTL_MAX;
			p->ySpeed = ySpeed / length * Y_SPEED_FULL * p->ttl / TTL_MAX;
			
			p->xPos += p->xSpeed;
			p->yPos -= p->ySpeed;

			// erase if die
			if (--(p->ttl) <= 0 || p->xPos < 0 || p->xPos >= WIDTH || p->yPos < 0 || p->yPos >= HEIGHT) {
				particles.erase(p--);
				continue;
			}
		}

		Mat scene = Mat::zeros(Size(WIDTH, HEIGHT), CV_8UC3);

		for (Particle& p : particles) {
			float alpha = float(p.ttl) / TTL_MAX;
			scene.at<Vec3b>(p.xPos, p.yPos) = scene.at<Vec3b>(p.xPos, p.yPos) * (1 - alpha) + Vec3b(255, 255, 255) * alpha;
		}
		imshow("Scene", scene);
		waitKey(1);
	}

	return EXIT_SUCCESS;
}