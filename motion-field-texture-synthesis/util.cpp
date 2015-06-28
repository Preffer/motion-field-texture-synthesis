#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <boost/format.hpp>
#include "util.hpp"

using namespace std;
using namespace boost;

const char* readFileBytes(const char* path) {
	FILE* fp = fopen(path, "r");
	if (!fp) {
		throw runtime_error((format("File not found: %1%") % path).str());
	}

	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	rewind(fp);

	char* buffer = new char[size + 1];
	fread(buffer, size, 1, fp);
	fclose(fp);
	buffer[size] = 0;

	return buffer;
}

const unsigned char* readBMP(const char* path) {
	FILE * fp = fopen(path, "rb");
	if (!fp) {
		throw runtime_error((format("File not found: %1%") % path).str());
	}

	unsigned char header[54];
	if (fread(header, 1, 54, fp) != 54){
		throw runtime_error("Not a correct BMP file");
	}

	// A BMP files always begins with "BM"
	if (header[0] != 'B' || header[1] != 'M'){
		throw runtime_error("Not a correct BMP file");
	}
	// Make sure this is a 24bpp file
	if (*(int*)&(header[0x1E]) != 0) {
		throw runtime_error("Not a correct BMP file");
	}
	if (*(int*)&(header[0x1C]) != 24) {
		throw runtime_error("Not a correct BMP file");
	}

	// Read the information about the image
	unsigned int dataPos = *(int*)&(header[0x0A]);
	unsigned int imageSize = *(int*)&(header[0x22]);
	unsigned int width = *(int*)&(header[0x12]);
	unsigned int height = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	// 3 : one byte for each Red, Green and Blue component
	// The BMP header is done that way
	if (imageSize == 0) {
		imageSize = width*height * 3;
	}
	if (dataPos == 0) {
		dataPos = 54;
	}

	// Create a buffer
	unsigned char* data = new unsigned char[imageSize];

	// Read the actual data from the file into the buffer
	fread(data, 1, imageSize, fp);

	// Everything is in memory now, the file wan be closed
	fclose(fp);

	return data;
}

GLuint buildShader(const char* shaderCode, GLenum shaderType) {
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderCode, NULL);
	glCompileShader(shader);

	GLint result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result != GL_TRUE) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &result);
		char* info = new char[result];
		glGetShaderInfoLog(shader, result, NULL, info);
		throw runtime_error((format("Build shader failed: %1%") % info).str());
	}

	return shader;
}

GLuint buildProgram(const char** shaderCodeArray, GLenum* shaderTypeArray, GLuint length) {
	GLuint program = glCreateProgram();
	for (GLuint i = 0; i < length; i++) {
		glAttachShader(program, buildShader(shaderCodeArray[i], shaderTypeArray[i]));
	}
	glLinkProgram(program);

	GLint result;
	glGetProgramiv(program, GL_LINK_STATUS, &result);
	if (result != GL_TRUE) {
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &result);
		char* info = new char[result];
		glGetProgramInfoLog(program, result, NULL, info);
		throw runtime_error((format("Build program failed: %1%") % info).str());
	}

	return program;
}

void checkErrors(const char* desc) {
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		cerr << format("OpenGL error in %1%: %2% (%3%)") % desc % gluErrorString(error) % error << endl;
	}
}
