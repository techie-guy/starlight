#pragma once

#include "VertexAttributes.h"
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
void getSpriteUV(Quad* quad, SpriteSheet* spriteSheet, Sprite* sprite);
void getSpriteAnimation(Quad* quad, SpriteSheet* spriteSheet, Sprite* sprite, int frame);
void destroySpriteSheet();
