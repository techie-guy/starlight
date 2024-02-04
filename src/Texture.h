#pragma once

#if defined(_PLATFORM_DESKTOP)
	#include <glad/gles2.h>
#elif defined(_PLATFORM_WEB)
	#include <emscripten.h>
	#include <GLES3/gl3.h>
#elif defined(_PLATFORM_ANDROID)
	#include <GLES3/gl3.h>
#endif

typedef struct
{
	unsigned int texture_id;
	int width;
	int height;
} Texture;

void init_texture_from_file(Texture* texture, const char* texture_path);
void init_texture_from_data(Texture* texture, GLint level, GLint internalformat, GLenum format, GLenum type, void* texture_data);
void set_texture_parameteri(GLenum target, GLenum pname, GLint param);
void bind_texture(Texture* texture);
void unbind_texture();
void destroy_texture(Texture* texture);
