#pragma once

#if defined(_PLATFORM_DESKTOP)
	#include <glad/gles2.h>
#elif defined(_PLATFORM_WEB)
	#include <emscripten.h>
	#include <GLES3/gl3.h>
#elif defined(_PLATFORM_ANDROID)
	#include <GLES3/gl3.h>
#endif

#include <stdbool.h>
#include <stb_ds.h>

#include "../utils/utils.h"

#include <cglm/cglm.h>
#include <cglm/struct.h>



// Vertex Attributes
typedef struct
{
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
} VertexAttributes;

typedef struct { char* key; VertexAttributes value; } VertexAttribHashMap;

enum Buffers
{
	VAO,
	VBO,
	EBO
};

void init_vertex_attributes(VertexAttributes* attrib, void* vertex_data, size_t vertex_data_size, void* index_data, size_t index_data_size, bool indexed);
void bind_vertex_buffer(VertexAttributes* attrib, enum Buffers buffer);
void unbind_vertex_buffer(VertexAttributes* attrib, enum Buffers buffer);
void destroy_vertex_attributes(VertexAttributes* attrib, bool indexed);


// Shader Program
void init_shader_program(unsigned int* shader_program, const char* vertex_shader_source_path, const char* fragment_shader_source_path);
void bind_shader_program(unsigned int* shader_program);

int get_uniform_location(unsigned int* shader_program, const char* uniform_name);

void uniform_int(unsigned int* shader_program, const char* uniform_name, int data);
void uniform_float(unsigned int* shader_program, const char* uniform_name, float data);
void uniform_vec2(unsigned int* shader_program, const char* uniform_name, vec2s data);
void uniform_vec3(unsigned int* shader_program, const char* uniform_name, vec3s data);
void uniform_vec4(unsigned int* shader_program, const char* uniform_name, vec4s data);
void uniform_mat3(unsigned int* shader_program, const char* uniform_name, mat3s data);
void uniform_mat4(unsigned int* shader_program, const char* uniform_name, mat4s data);

void destroy_shader_program(unsigned int* shader_program);


// Texture Loading

typedef struct
{
	bool is_init;

	GLint min_filter;
	GLint mag_filter;
	GLint wrap_s;
	GLint wrap_t;
} TextureConfig;

typedef struct
{
	unsigned int texture_id;
	int width;
	int height;

	TextureConfig texture_config;
} Texture;

typedef struct { char* key; Texture value; } TextureHashMap;

void init_texture_from_file(Texture* texture, const char* texture_path);
void init_texture_from_data(Texture* texture, GLint level, GLint internalformat, GLenum format, GLenum type, void* texture_data);

void add_texture_from_file(TextureHashMap** textures, const char* texture_name, const char* texture_path);
void add_texture_from_data(TextureHashMap** textures, const char* texture_name, GLint level, GLint internalformat, GLenum format, GLenum type, void* texture_data);

void texture_active_slot(GLenum slot);
void set_texture_parameteri(GLenum target, GLenum pname, GLint param);
void bind_texture(Texture* texture);
void unbind_texture();
void destroy_texture(Texture* texture);
void destroy_textures(TextureHashMap* textures);
