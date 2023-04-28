#pragma once

typedef struct
{
	unsigned int textureId;
	int width;
	int height;
	int channels;
} Texture;

void initTexture(Texture* texture, const char* texturePath);
void bindTexture(Texture* texture);
void destroyTexture(Texture* texture);
