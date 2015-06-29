#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <boost/format.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "util.hpp"

using namespace std;
using namespace boost;
using namespace glm;

const static int WIDTH = 512;
const static int HEIGHT = 512;
const static int COUNT = 160;
const static int DEPTH = 1024;
const static int BATCH = 4;

int main(int argc, char* argv[]) {
	if (!glfwInit()) {
		throw runtime_error("Failed to initialize GLFW");
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Render", NULL, NULL);
	if (!window) {
		throw runtime_error("Failed to open GLFW window");
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		throw runtime_error("Failed to initialize GLEW");
	}
	checkError("init");

	// motion
	const unsigned char* motionData = readBMP("motion.bmp");
	GLuint motion;
	glGenTextures(1, &motion);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, motion);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, motionData);
	glBindImageTexture(0, motion, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8UI);
	checkError("motion");

	// position
	/*float* positionData = new float[COUNT * COUNT * DEPTH * 4];
	for (int i = 0; i < COUNT * COUNT * DEPTH; i += 4) {
		positionData[i + 3] = 0;
	}*/

	GLuint position;
	glGenTextures(1, &position);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, position);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, COUNT, COUNT, DEPTH, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(1, position, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	checkError("position");

	// velocity
	/*float* velocityData = new float[COUNT * COUNT * 4];
	for (int i = 0; i < COUNT * COUNT; i += 4) {
		velocityData[i    ] = 0.5;
		velocityData[i + 1] = 0;
	}*/

	GLuint velocity;
	glGenTextures(1, &velocity);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D, velocity);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, COUNT, COUNT, DEPTH, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(2, velocity, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	checkError("velocity");

	// control
	float* controlData = new float[COUNT * COUNT * DEPTH * 4];
	for (int i = 0; i < (COUNT * COUNT * DEPTH * 4); i++) {
		controlData[i] = float(rand()) / RAND_MAX;
	}
	GLuint control;
	glGenTextures(1, &control);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_3D, control);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, COUNT, COUNT, DEPTH, 0, GL_RGBA, GL_FLOAT, controlData);
	glBindImageTexture(3, control, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	checkError("control");

	// display
	GLuint display;
	glGenTextures(1, &display);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, display);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindImageTexture(4, display, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8UI);
	glGenerateMipmap(GL_TEXTURE_2D);
	checkError("display");

	// compute shader
	const static char* computeSrc =
		#include "compute.shader"
	;
	GLenum computeType[] = { GL_COMPUTE_SHADER };
	GLuint compute = buildProgram(&computeSrc, computeType, 1);
	glUseProgram(compute);
	glUniform1i(glGetUniformLocation(compute, "depth"), DEPTH);
	glUniform1i(glGetUniformLocation(compute, "batch"), BATCH);
	glUniform2i(glGetUniformLocation(compute, "size"), WIDTH, HEIGHT);
	checkError("compute");

	// draw shader
	const static char* vertexSrc =
		#include "vertex.shader"
	;
	const static char* fragmentSrc =
		#include "fragment.shader"
	;
	const char* drawSrc[] = { vertexSrc, fragmentSrc };
	GLenum drawType[] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
	GLuint draw = buildProgram(drawSrc, drawType, 2);
	glUseProgram(draw);
	glUniform2i(glGetUniformLocation(draw, "size"), WIDTH, HEIGHT);
	checkError("draw");

	// draw data
	GLuint vertexArray;
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	const static GLfloat vertexData[] = {
		-1.0f, -1.0f,
		-1.0f,  1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f
	};

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);
	checkError("vertex");

	// main loop
	double lastTime = glfwGetTime();
	double nowTime;
	char fpsTitle[32];
	do {
		glUseProgram(compute);
		glDispatchCompute(COUNT / 16, COUNT / 16, 1);
		glUseProgram(draw);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(vertexData));
		glfwSwapBuffers(window);
		glfwPollEvents();
		glClearTexImage(display, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		nowTime = glfwGetTime();
		sprintf(fpsTitle, "Render - FPS: %.2f", 1 / (nowTime - lastTime));
		glfwSetWindowTitle(window, fpsTitle);
		lastTime = nowTime;
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
	
	return EXIT_SUCCESS;
}
