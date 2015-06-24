#include <iostream>
#include <list>
#include <boost/format.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace boost;
using namespace cv;

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
	xPos = 320;
	yPos = 320;
	xSpeed = float(rand()) / RAND_MAX - 0.5;
	ySpeed = float(rand()) / RAND_MAX - 0.5;
	ttl = rand() % 1000;
}

int main(int argc, char *argv[]) {
	Mat motion = imread(argv[1], CV_LOAD_IMAGE_COLOR);
	//imshow("Motion", motion);

	list<Particle> particles;

	/*
	VideoWriter videoOut(
		string(argv[1]) + ".mpg",
		CV_FOURCC('P','I','M','1'),
		60,
		Size(640, 640)
	);

	if(!videoOut.isOpened()){
		cerr << "Failed to open " << endl;
		return EXIT_FAILURE;
	}
	*/

	while (true) {
		for (int i = 0; i < 1000; i++) {
			particles.push_back(Particle());
		}

		// apply motion
		for (auto p = particles.begin(); p != particles.end(); p++) {
			if (--(p->ttl) <= 0) {
				particles.erase(p--);
				continue;
			} 
			Vec3b bgrPixel = motion.at<Vec3b>(p->xPos, p->yPos);

			p->xPos += p->xSpeed;
			p->yPos += p->ySpeed;

			if (p->xPos < 0 || p->xPos > 639 || p->yPos < 0 || p->yPos > 639) {
				particles.erase(p--);
				continue;
			}

			p->xSpeed += (float(bgrPixel[0]) - 128) / 10000.0;
			p->ySpeed += (float(bgrPixel[1]) - 128) / 10000.0;
		}

		Mat scene = Mat::zeros(motion.size(), motion.type());

		for (Particle& p : particles) {
			scene.at<Vec3b>(p.xPos, p.yPos) += (Vec3b(255, 255, 255) - scene.at<Vec3b>(p.xPos, p.yPos)) * (p.ttl / 1000.0);
		}

		//videoOut << scene;
		imshow("Scene", scene);
		waitKey(1);
	}

	return EXIT_SUCCESS;
}