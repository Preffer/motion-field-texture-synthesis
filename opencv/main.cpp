#include <iostream>
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
	xSpeed = float(rand()) / RAND_MAX;
	ySpeed = float(rand()) / RAND_MAX;
	ttl = rand() % 800;
}

int main(int argc, char *argv[]) {
	Mat motion = imread(argv[1], CV_LOAD_IMAGE_COLOR);
	//imshow("Motion", motion);

	vector<Particle> particles;

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

	while (true) {
		for (int i = 0; i < 10; i++) {
			particles.push_back(Particle());
		}

		// apply motion
		for (uint i = 0; i < particles.size(); i++) {
			Particle& p = particles[i];
			if (--(p.ttl) <= 0) {
				particles.erase(particles.begin() + (i--));
				continue;
			} 
			Vec3b bgrPixel = motion.at<Vec3b>(p.xPos, p.yPos);

			p.xPos += p.xSpeed;
			p.yPos += p.ySpeed;

			if (p.xPos <= 1 || p.xPos >= 638 || p.yPos <= 1 || p.yPos >= 638) {
				particles.erase(particles.begin() + (i--));
				continue;
			}

			p.xSpeed += (float(bgrPixel[0]) - 128) / 10000.0;
			p.ySpeed += (float(bgrPixel[1]) - 128) / 10000.0;
		}

		Mat scene = Mat::zeros(motion.size(), motion.type());
		
		for(Particle& p : particles) {
			scene.at<Vec3b>(p.xPos, p.yPos) = Vec3b(255, 255, 255) * (p.ttl / 800.0);
		}
		videoOut << scene;
		imshow("Scene", scene);
		waitKey(1);
	}

	return EXIT_SUCCESS;
}