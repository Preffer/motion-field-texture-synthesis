#ifndef UTIL_HPP
#define UTIL_HPP

#include <GL/glew.h>

const char* readFileBytes(const char* path);
const unsigned char* readBMP(const char* path);

GLuint buildShader(const char* shaderCode, GLenum shaderType);
GLuint buildProgram(const char** shaderCodeArray, GLenum* shaderTypeArray, GLuint length);

void checkErrors(const char* desc);

#endif
