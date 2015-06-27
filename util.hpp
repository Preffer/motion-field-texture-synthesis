#ifndef UTIL_HPP
#define UTIL_HPP

const char* readFileBytes(const char* path);
GLuint readImage(const char* path, int* w, int* h);
GLuint buildShader(const char* shaderFile, GLenum shaderType);
GLuint buildProgram(const char* vertexShaderFile, const char* fragmentShaderFile);

GLuint genRenderProg(GLuint texHandle);
GLuint genTexture();
void checkErrors(std::string desc);
GLuint genComputeProg(GLuint texHandle);

#endif
