#include "opengl.h"
#include <iostream>
#include <boost/format.hpp>

using namespace std;
using namespace boost;

GLuint renderHandle, computeHandle;

void updateTex(int);
void draw();

int main() {
	GLFWwindow* window = initGL();

	GLuint texHandle = genTexture();
	renderHandle = genRenderProg(texHandle);
	computeHandle = genComputeProg(texHandle);

	double frameCount = 0;
	do {
		for (int i = 0; i < 1024; ++i) {
			updateTex(i);
			draw();
			cout << format("Time: %1%, Count: %2%, FPS: %3%") % glfwGetTime() % (++frameCount) % (frameCount / glfwGetTime()) << endl;
		}
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	return 0;
}

void updateTex(int frame) {
	glUseProgram(computeHandle);
	glUniform1f(glGetUniformLocation(computeHandle, "roll"), (float)frame*0.01f);
	glDispatchCompute(512 / 16, 512 / 16, 1); // 512^2 threads in blocks of 16^2
	checkErrors("Dispatch compute shader");
}

void draw() {
	glUseProgram(renderHandle);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	swapBuffers();
	checkErrors("Draw screen");
}
