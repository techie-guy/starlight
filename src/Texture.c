#include "Texture.h"
#include "Utils.h"

#include <stb_image.h>
#include <stb_ds.h>


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
