#pragma once

#include <cglm/cglm.h>
#include <cglm/struct.h>

void initShaderProgram(unsigned int* shaderProgram, const char* vertexShaderSourcePath, const char* fragmentShaderSourcePath);
void bindShaderProgram(unsigned int* shaderProgram);

int getUniformLocation(unsigned int* shaderProgram, const char* uniformName);

void uniformInt(unsigned int* shaderProgram, const char* uniformName, int data);
void uniformFloat(unsigned int* shaderProgram, const char* uniformName, float data);
void uniformVec2(unsigned int* shaderProgram, const char* uniformName, vec2s data);
void uniformVec3(unsigned int* shaderProgram, const char* uniformName, vec3s data);
void uniformVec4(unsigned int* shaderProgram, const char* uniformName, vec4s data);
void uniformMat4(unsigned int* shaderProgram, const char* uniformName, mat4s data);

void destroyShaderProgram(unsigned int* shaderProgram);
