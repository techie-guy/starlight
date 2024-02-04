#include "Texture.h"
#include "Utils.h"

#include <stb_image.h>

void init_texture_from_file(Texture* texture, const char* texture_path)
{
	stbi_set_flip_vertically_on_load(true);

	int channels;

	unsigned char* texture_data = stbi_load(texture_path, &texture->width, &texture->height, &channels, 0);
	if(!texture_data)
	{
		log_error("Invalid Texture Data: %s\n", texture_path);
	}

	init_texture_from_data(texture, 0, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);

	set_texture_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	set_texture_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	set_texture_parameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	set_texture_parameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_image_free(texture_data);
}

void init_texture_from_data(Texture* texture, GLint level, GLint internalformat, GLenum format, GLenum type, void* texture_data)
{
	glGenTextures(1, &texture->texture_id);
	bind_texture(texture);

	glTexImage2D(GL_TEXTURE_2D, level, internalformat, texture->width, texture->height, 0, format, type, texture_data);
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
