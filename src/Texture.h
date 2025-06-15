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
