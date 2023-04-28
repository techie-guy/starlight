#pragma once

#include <cglm/struct.h>
#include <hashmap.h>

typedef struct
{
	char* name;
	int x;
	int y;
	int frameSpeed;
	int spriteCount;
	int spriteWidth;
	int spriteHeight;
} Sprite;

typedef struct
{
	char* name;
	char* path;
	char* defaultSprite;
	int width;
	int height;
	struct hashmap* spriteHashMap;
} SpriteSheet;

extern struct hashmap* spriteSheetHashMap;

void initSpriteSheet();
void destroySpriteSheet();
