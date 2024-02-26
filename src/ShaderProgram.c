#include "ShaderProgram.h"
#include "Utils.h"

#if defined(_PLATFORM_DESKTOP)
	#include <glad/gles2.h>
#elif defined(_PLATFORM_WEB)
	#include <emscripten.h>
	#include <GLES3/gl3.h>
#elif defined(_PLATFORM_ANDROID)
	#include <GLES3/gl3.h>
#endif


void check_for_errors(unsigned int id, int status)
{
	int success;
	char info_log[512];

	if(status == GL_COMPILE_STATUS)
	{
		glGetShaderiv(id, status, &success);

		if(!success)
		{
			glGetShaderInfoLog(id, 512, NULL, info_log);
			log_error("Error in Compiling Shader\n%s\n", info_log);
			exit(EXIT_FAILURE);
		}
	}
	else if(status == GL_LINK_STATUS)
	{
		glGetProgramiv(id, status, &success);

		if(!success)
		{
			glGetProgramInfoLog(id, 512, NULL, info_log);
			log_error("Error in Linking Shaders\n%s\n", info_log);
			exit(EXIT_FAILURE);
		}
	}
}

void compile_shader(unsigned int* shader, const int shader_type, const char** shader_source)
{
	*shader = glCreateShader(shader_type);
	glShaderSource(*shader, 1, shader_source, NULL);
	glCompileShader(*shader);

	check_for_errors(*shader, GL_COMPILE_STATUS);
}

void init_shader_program(unsigned int* shader_program, const char* vertex_shader_source_path, const char* fragment_shader_source_path)
{
	char* vertex_shader_source = read_file(vertex_shader_source_path, "r");
	char* fragment_shader_source = read_file(fragment_shader_source_path, "r");

	unsigned int vertex_shader;
	compile_shader(&vertex_shader, GL_VERTEX_SHADER, (const char**)&vertex_shader_source);

	unsigned int fragment_shader;
	compile_shader(&fragment_shader, GL_FRAGMENT_SHADER, (const char**)&fragment_shader_source);

	*shader_program = glCreateProgram();
	glAttachShader(*shader_program, vertex_shader);
	glAttachShader(*shader_program, fragment_shader);
	glLinkProgram(*shader_program);

	check_for_errors(*shader_program, GL_LINK_STATUS);

	free(vertex_shader_source);
	free(fragment_shader_source);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

void bind_shader_program(unsigned int* shader_program)
{
	glUseProgram(*shader_program);
}

int get_uniform_location(unsigned int* shader_program, const char* uniform_name)
{
	return glGetUniformLocation(*shader_program, uniform_name);
}

void uniform_int(unsigned int* shader_program, const char* uniform_name, int data)
{
	glUniform1i(get_uniform_location(shader_program, uniform_name), data);
}

void uniform_float(unsigned int* shader_program, const char* uniform_name, float data)
{
	glUniform1f(get_uniform_location(shader_program, uniform_name), data);
}

void uniform_vec2(unsigned int* shader_program, const char* uniform_name, vec2s data)
{
	glUniform2fv(get_uniform_location(shader_program, uniform_name), 1, data.raw);
}

void uniform_vec3(unsigned int* shader_program, const char* uniform_name, vec3s data)
{
	glUniform3fv(get_uniform_location(shader_program, uniform_name), 1, data.raw);
}

void uniform_vec4(unsigned int* shader_program, const char* uniform_name, vec4s data)
{
	glUniform4fv(get_uniform_location(shader_program, uniform_name), 1, data.raw);
}

void uniform_mat4(unsigned int* shader_program, const char* uniform_name, mat4s data)
{
	glUniformMatrix4fv(get_uniform_location(shader_program, uniform_name), 1, GL_FALSE, (float*)data.raw);
}

void destroy_shader_program(unsigned int* shader_program)
{
	glDeleteProgram(*shader_program);
}
