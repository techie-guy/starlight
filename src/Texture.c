#include "Texture.h"
#include "Utils.h"

#include <stb_image.h>

void initTextureFromFile(Texture* texture, const char* texturePath)
{
	stbi_set_flip_vertically_on_load(true);

	int channels;

	unsigned char* textureData = stbi_load(texturePath, &texture->width, &texture->height, &channels, 0);
	if(!textureData)
	{
		log_error("Invalid Texture Data: %s", texturePath);
	}

	initTextureFromData(texture, 0, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, textureData);

	setTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	setTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	setTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	setTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_image_free(textureData);
}

void initTextureFromData(Texture* texture, GLint level, GLint internalformat, GLenum format, GLenum type, void* textureData)
{
	glGenTextures(1, &texture->textureId);
	bindTexture(texture);

	glTexImage2D(GL_TEXTURE_2D, level, internalformat, texture->width, texture->height, 0, format, type, textureData);	
}

void setTextureParameteri(GLenum target, GLenum pname, GLint param)
{
	glTexParameteri(target, pname, param);
}

void bindTexture(Texture* texture)
{
	glBindTexture(GL_TEXTURE_2D, texture->textureId);
}

void unbindTexture()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void destroyTexture(Texture* texture)
{
	glDeleteTextures(1, &texture->textureId);
}
