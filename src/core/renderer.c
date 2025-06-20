#include "renderer.h"

#include <stb_image.h>
#include <stb_ds.h>

// Vertex Attributes
void init_vertex_attributes(VertexAttributes* attrib, void* vertex_data, size_t vertex_data_size, void* index_data, size_t index_data_size, bool indexed)
{
	glGenVertexArrays(1, &attrib->VAO);
	bind_vertex_buffer(attrib, VAO);

	glGenBuffers(1, &attrib->VBO);
	bind_vertex_buffer(attrib, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertex_data_size, vertex_data, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(3);

	if(indexed)
	{
		glGenBuffers(1, &attrib->EBO);
		bind_vertex_buffer(attrib, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_data_size, index_data, GL_DYNAMIC_DRAW);
	}
}

void bind_vertex_buffer(VertexAttributes* attrib, enum Buffers buffer)
{
	switch(buffer)
	{
		case VAO:
			glBindVertexArray(attrib->VAO);
		case VBO:
			glBindBuffer(GL_ARRAY_BUFFER, attrib->VBO);
		case EBO:
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, attrib->EBO);
	}
}

void unbind_vertex_buffer(VertexAttributes* attrib, enum Buffers buffer)
{
	switch(buffer)
	{
		case VAO:
			glBindVertexArray(0);
		case VBO:
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		case EBO:
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void destroy_vertex_attributes(VertexAttributes* attrib, bool indexed)
{
	glDeleteVertexArrays(1, &attrib->VAO);
	glDeleteBuffers(1, &attrib->VBO);

	if(indexed)
	{
		glDeleteBuffers(1, &attrib->EBO);
	}
}


// Shader Program
void shader_check_for_errors(unsigned int id, int status)
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

	shader_check_for_errors(*shader, GL_COMPILE_STATUS);
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

	shader_check_for_errors(*shader_program, GL_LINK_STATUS);

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

void uniform_mat3(unsigned int* shader_program, const char* uniform_name, mat3s data)
{
	glUniformMatrix3fv(get_uniform_location(shader_program, uniform_name), 1, GL_FALSE, (float*)data.raw);
}

void destroy_shader_program(unsigned int* shader_program)
{
	glDeleteProgram(*shader_program);
}


// Texture Loading
void init_texture_from_file(Texture* texture, const char* texture_path)
{
	stbi_set_flip_vertically_on_load(true);

	int channels;

	unsigned char* texture_data = stbi_load(texture_path, &texture->width, &texture->height, &channels, 4);
	if(!texture_data)
	{
		log_error("Invalid Texture Data: %s\n", texture_path);
	}

	init_texture_from_data(texture, 0, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);
	
	stbi_image_free(texture_data);
}

void init_texture_from_data(Texture* texture, GLint level, GLint internalformat, GLenum format, GLenum type, void* texture_data)
{
	glGenTextures(1, &texture->texture_id);
	bind_texture(texture);

	if(!texture->texture_config.is_init)
	{
		texture->texture_config.wrap_s = GL_REPEAT;
		texture->texture_config.wrap_t = GL_REPEAT;
		texture->texture_config.min_filter = GL_NEAREST;
		texture->texture_config.mag_filter = GL_NEAREST;
	}	

	set_texture_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture->texture_config.wrap_s);
	set_texture_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture->texture_config.wrap_t);
	set_texture_parameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->texture_config.min_filter);
	set_texture_parameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->texture_config.mag_filter);

	glTexImage2D(GL_TEXTURE_2D, level, internalformat, texture->width, texture->height, 0, format, type, texture_data);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void add_texture_from_file(TextureHashMap** textures, const char* texture_name, const char* texture_path)
{
	Texture texture = {};

	init_texture_from_file(&texture, texture_path);

	shput(*textures, texture_name, texture);
}


void add_texture_from_data(TextureHashMap** textures, const char* texture_name, GLint level, GLint internalformat, GLenum format, GLenum type, void* texture_data)
{
	Texture texture = {};

	init_texture_from_data(&texture, level, internalformat, format, type, texture_data);

	shput(*textures, texture_name, texture);
}

void texture_active_slot(GLenum slot)
{
	glActiveTexture(slot);
}


void set_texture_parameteri(GLenum target, GLenum pname, GLint param)
{
	glTexParameteri(target, pname, param);
}

void bind_texture(Texture* texture)
{
	glBindTexture(GL_TEXTURE_2D, texture->texture_id);
}

void unbind_texture()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void destroy_texture(Texture* texture)
{
	glDeleteTextures(1, &texture->texture_id);
}

void destroy_textures(TextureHashMap* textures)
{
	for(int i = 0; i < shlen(textures); i++)
	{
		//
	}
}
