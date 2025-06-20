#pragma once

#include "utils.h"

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

typedef struct
{
	float x1;
	float x2;
	float y1;
	float y2;
} UV_Coords;

extern struct hashmap* sprite_sheet_hashmap;

void init_spritesheet();
void get_spriteUV(UV_Coords* uv_coords, SpriteSheet* spritesheet, Sprite* sprite);
void get_sprite_animation_vert(Vertex vertex_render_data[], int start, SpriteSheet* spritesheet, Sprite* sprite, int frame);
void get_sprite_animation(Quad* quad, SpriteSheet* spritesheet, Sprite* sprite, int frame);
void destroy_spriteSheet();
