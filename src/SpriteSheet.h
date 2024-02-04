#pragma once

#include "VertexAttributes.h"
#include <cglm/struct.h>
#include <hashmap.h>

typedef struct
{
	char* name;
	int x;
	int y;
	int frame_speed;
	int sprite_count;
	int sprite_width;
	int sprite_height;
} Sprite;

enum SpriteSheetType
{
	SHEET_HORIZONTAL = 0,
	SHEET_VERTICAL = 1,
};

typedef struct
{
	char* name;
	char* path;
	char* default_sprite;
	enum SpriteSheetType type;
	int width;
	int height;
	struct hashmap* sprite_hashmap;
} SpriteSheet;

extern struct hashmap* sprite_sheet_hashmap;

void init_spritesheet();
void get_spriteUV(Quad* quad, SpriteSheet* spritesheet, Sprite* sprite);
void get_sprite_animation(Quad* quad, SpriteSheet* spritesheet, Sprite* sprite, int frame);
void destroy_spriteSheet();
