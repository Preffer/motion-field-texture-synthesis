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
const int XPOS_BASE = 256;
const int YPOS_BASE = 256;
const int XPOS_VARY = 30;
const int YPOS_VARY = 30;
const int XSPEED_BASE = 10;
const int YSPEED_BASE = 10;
const int XSPEED_VARY = 5;
const int YSPEED_VARY = 5;
const int TTL_BASE = 800;
const int TTL_VARY = 10;
const int TTL_MAX = 1000;
const int BORN_SPEED = 300;
const float XSPEED = 2;
const float YSPEED = 2.2;
const float WEIGHT = 0.025;

class Particle {
public:
	Particle();
	float xPos;
	float yPos;
	int ttl;
};

Particle::Particle() {
	xPos = BORN_X + float(rand()) * BORN_X_VARY / RAND_MAX;
	yPos = BORN_Y + float(rand()) * BORN_Y_VARY / RAND_MAX;
	ttl  = rand() % AGE;
}

int main(int argc, char* argv[]) {
	Mat motion = imread("move.bmp", CV_LOAD_IMAGE_COLOR);
	//imshow("Motion", motion);

	list<Particle> particles;

	while (true) {
		for (int i = 0; i < BORN; i++) {
			particles.push_back(Particle());
		}

		// apply motion
		for (auto p = particles.begin(); p != particles.end(); p++) {
			if (--(p->ttl) <= 0) {
				particles.erase(p--);
				continue;
			}
			Vec3b bgrPixel = motion.at<Vec3b>(p->xPos, p->yPos);
			p->xPos += (float(rand()) / RAND_MAX - 0.9) * XSPEED;
			p->yPos += (float(rand()) / RAND_MAX + 0.6) * YSPEED;
			
			p->xPos -= (float(bgrPixel[2]) - 0) * WEIGHT;
			p->yPos -= (float(bgrPixel[1]) - 0) * WEIGHT;
			if (p->xPos < 0 || p->xPos >= WIDTH || p->yPos < 0 || p->yPos >= HEIGHT) {
				particles.erase(p--);
				continue;
			}
		}

		Mat scene = Mat::zeros(Size(WIDTH, HEIGHT), CV_8UC3);

		for (Particle& p : particles) {
			scene.at<Vec3b>(p.xPos, p.yPos) = Vec3b(255, 255, 255) * (float(p.ttl) / AGE);
		}
		imshow("Scene", scene);
		waitKey(1);
	}

	return EXIT_SUCCESS;
}