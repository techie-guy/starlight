#include "ShaderProgram.h"
#include "Utils.h"

#if defined(_PLATFORM_NATIVE)
	#include <glad/gles2.h>
#elif defined(_PLATFORM_WEB)
	#include <GLES3/gl3.h>
#endif


void checkForErrors(unsigned int id, int status)
{
	int success;
	char infoLog[512];

	if(status == GL_COMPILE_STATUS)
	{
		glGetShaderiv(id, status, &success);

		if(!success)
		{
			glGetShaderInfoLog(id, 512, NULL, infoLog);
			log_error("Error in Compiling Shader\n%s\n", infoLog);
			exit(EXIT_FAILURE);
		}
	}
	else if(status == GL_LINK_STATUS)
	{
		glGetProgramiv(id, status, &success);

		if(!success)
		{
			glGetProgramInfoLog(id, 512, NULL, infoLog);
			log_error("Error in Linking Shaders\n%s\n", infoLog);
			exit(EXIT_FAILURE);
		}
	}
}

void compileShader(unsigned int* shader, const int shaderType, const char** shaderSource)
{
	*shader = glCreateShader(shaderType);
	glShaderSource(*shader, 1, shaderSource, NULL);
	glCompileShader(*shader);

	checkForErrors(*shader, GL_COMPILE_STATUS);
}

void initShaderProgram(unsigned int* shaderProgram, const char* vertexShaderSourcePath, const char* fragmentShaderSourcePath)
{
	char* vertexShaderSource = readFile(vertexShaderSourcePath);
	char* fragmentShaderSource = readFile(fragmentShaderSourcePath);

	unsigned int vertexShader;
	compileShader(&vertexShader, GL_VERTEX_SHADER, (const char**)&vertexShaderSource);

	unsigned int fragmentShader;
	compileShader(&fragmentShader, GL_FRAGMENT_SHADER, (const char**)&fragmentShaderSource);

	*shaderProgram = glCreateProgram();
	glAttachShader(*shaderProgram, vertexShader);
	glAttachShader(*shaderProgram, fragmentShader);
	glLinkProgram(*shaderProgram);

	checkForErrors(*shaderProgram, GL_LINK_STATUS);

	free(vertexShaderSource);
	free(fragmentShaderSource);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void bindShaderProgram(unsigned int* shaderProgram)
{
	glUseProgram(*shaderProgram);
}

int getUniformLocation(unsigned int* shaderProgram, const char* uniformName)
{
	return glGetUniformLocation(*shaderProgram, uniformName);
}

void uniformInt(unsigned int* shaderProgram, const char* uniformName, int data)
{
	glUniform1i(getUniformLocation(shaderProgram, uniformName), data);
}

void uniformFloat(unsigned int* shaderProgram, const char* uniformName, float data)
{
	glUniform1f(getUniformLocation(shaderProgram, uniformName), data);
}

void uniformVec2(unsigned int* shaderProgram, const char* uniformName, vec2s data)
{
	glUniform2fv(getUniformLocation(shaderProgram, uniformName), 1, data.raw);
}

void uniformVec3(unsigned int* shaderProgram, const char* uniformName, vec3s data)
{
	glUniform3fv(getUniformLocation(shaderProgram, uniformName), 1, data.raw);
}

void uniformVec4(unsigned int* shaderProgram, const char* uniformName, vec4s data)
{
	glUniform4fv(getUniformLocation(shaderProgram, uniformName), 1, data.raw);
}

void uniformMat4(unsigned int* shaderProgram, const char* uniformName, mat4s data)
{
	glUniformMatrix4fv(getUniformLocation(shaderProgram, uniformName), 1, GL_FALSE, (float*)data.raw);
}

void destroyShaderProgram(unsigned int* shaderProgram)
{
	glDeleteProgram(*shaderProgram);
}
