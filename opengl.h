#ifndef _OPENGL_H
#define _OPENGL_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

GLFWwindow* initGL();
void swapBuffers();

// Return handles
GLuint genTexture();
GLuint genRenderProg(GLuint); // Texture as the param
GLuint genComputeProg(GLuint);

void checkErrors(std::string);

#endif
