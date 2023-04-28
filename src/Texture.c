#include "Texture.h"
#include "Utils.h"

#if defined(_PLATFORM_NATIVE)
	#include <glad/gles2.h>
#elif defined(_PLATFORM_WEB)
	#include <GLES3/gl3.h>
#endif

#include <stb_image.h>

void initTexture(Texture* texture, const char* texturePath)
{
	glGenTextures(1, &texture->textureId);
	bindTexture(texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_set_flip_vertically_on_load(true);

	unsigned char* textureData = stbi_load(texturePath, &texture->width, &texture->height, &texture->channels, 0);
	if(textureData)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
	}
	else
	{
		log_error("Couldn't load texture: %s", texturePath);
	}

	stbi_image_free(textureData);
}

void bindTexture(Texture* texture)
{
	glBindTexture(GL_TEXTURE_2D, texture->textureId);
}

void destroyTexture(Texture* texture)
{
	glDeleteTextures(1, &texture->textureId);
}
