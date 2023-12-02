#pragma once

#if defined(_PLATFORM_NATIVE)
	#include <glad/gles2.h>
#elif defined(_PLATFORM_WEB)
	#include <GLES3/gl3.h>
#endif

typedef struct
{
	unsigned int textureId;
	int width;
	int height;
} Texture;

void initTextureFromFile(Texture* texture, const char* texturePath);
void initTextureFromData(Texture* texture, GLint level, GLint internalformat, GLenum format, GLenum type, void* textureData);
void setTextureParameteri(GLenum target, GLenum pname, GLint param);
void bindTexture(Texture* texture);
void unbindTexture();
void destroyTexture(Texture* texture);
