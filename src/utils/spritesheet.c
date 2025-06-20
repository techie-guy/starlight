#include "spritesheet.h"
#include "utils.h"
#include "json_helper.h"

#include <string.h>
#include <hashmap.h>

struct hashmap* sprite_sheet_hashmap;

int sprite_sheet_compare(const void* a, const void* b, void* udata)
{
	const SpriteSheet* ua = a;
	const SpriteSheet* ub = b;
	return strcmp(ua->name, ub->name);
}

int sprite_compare(const void* a, const void* b, void* udata)
{
	const Sprite* ua = a;
	const Sprite* ub = b;
	return strcmp(ua->name, ub->name);
}

uint64_t sprite_sheet_hash(const void* item, uint64_t seed0, uint64_t seed1)
{
	const SpriteSheet* sheet = item;
	return hashmap_sip(sheet->name, strlen(sheet->name), seed0, seed1);
}

uint64_t sprite_hash(const void* item, uint64_t seed0, uint64_t seed1)
{
	const Sprite* sprite = item;
	return hashmap_sip(sprite->name, strlen(sprite->name), seed0, seed1);
}

void init_spritesheet()
{
	sprite_sheet_hashmap = hashmap_new(sizeof(SpriteSheet), 0, 0, 0, sprite_sheet_hash, sprite_sheet_compare, NULL, NULL);

	char* texture_json_string = read_file("spritesheets.json", "r");
	cJSON* texture_json = json_parse(texture_json_string);

	cJSON* spritesheets_json = json_get_object(texture_json, "spritesheets");

	cJSON* spriteSheet_json;
	cJSON_ArrayForEach(spriteSheet_json, spritesheets_json)
	{
		char* sheet_name = strdup(json_get_string(spriteSheet_json, "name"));
		char* sheet_path = strdup(json_get_string(spriteSheet_json, "path"));
		char* sheet_default_sprite = strdup(json_get_string(spriteSheet_json, "default_sprite"));
		int sheet_width = json_get_int(spriteSheet_json, "width");
		int sheet_height = json_get_int(spriteSheet_json, "height");
		int sheet_type = json_get_int(spriteSheet_json, "type");

		struct hashmap* sprite_hashmap = hashmap_new(sizeof(Sprite), 0, 0, 0, sprite_hash, sprite_compare, NULL, NULL);

		cJSON* sprites_json = json_get_object(spriteSheet_json, "sprites");

		cJSON* sprite_json;

		cJSON_ArrayForEach(sprite_json, sprites_json)
		{
			char* sprite_name = strdup(json_get_string(sprite_json, "name"));
			int sprite_x = json_get_int(sprite_json, "x");
			int sprite_y = json_get_int(sprite_json, "y");
			int sprite_frame_speed = json_get_int(sprite_json, "frame_speed");
			int sprite_count = json_get_int(sprite_json, "sprite_count");
			int sprite_width = json_get_int(sprite_json, "sprite_width");
			int sprite_height = json_get_int(sprite_json, "sprite_height");

			hashmap_set(sprite_hashmap, &(Sprite){
				.name=sprite_name,
				.x=sprite_x, .y=sprite_y,
				.frame_speed=sprite_frame_speed,
				.sprite_count=sprite_count,
				.sprite_width=sprite_width, .sprite_height=sprite_height,
			});
		}

		hashmap_set(sprite_sheet_hashmap, &(SpriteSheet){
			.name=sheet_name,
			.path=sheet_path,
			.type=sheet_type,
			.default_sprite=sheet_default_sprite,
			.width=sheet_width, .height=sheet_height,
			.sprite_hashmap=sprite_hashmap
		});
	}

	json_delete_object(texture_json);
	free(texture_json_string);
}

void get_sprite_animation(Quad* quad, SpriteSheet* spritesheet, Sprite* sprite, int frame)
{
	float x1, x2, y1, y2;

	if(spritesheet->type == SHEET_HORIZONTAL)
	{
		x1 = (float)(frame * sprite->sprite_width)/spritesheet->width;
		x2 = (float)((frame + 1) * sprite->sprite_width)/spritesheet->width;
		y1 = (float)(sprite->y * sprite->sprite_height)/spritesheet->height;
		y2 = (float)((sprite->y + 1) * sprite->sprite_height)/spritesheet->height;
	}
	if(spritesheet->type == SHEET_VERTICAL)
	{
		x1 = (float)(sprite->x * sprite->sprite_width)/spritesheet->width;
		x2 = (float)((sprite->x + 1) * sprite->sprite_width)/spritesheet->width;
		y1 = (float)(frame * sprite->sprite_height)/spritesheet->height;
		y2 = (float)((frame + 1) * sprite->sprite_height)/spritesheet->height;
	}
	

	quad->vertices[0].tex_coord = (vec2s){{x2, y2}};
	quad->vertices[1].tex_coord = (vec2s){{x2, y1}};
	quad->vertices[2].tex_coord = (vec2s){{x1, y1}};
	quad->vertices[3].tex_coord = (vec2s){{x1, y2}};
}


void get_sprite_animation_vert(Vertex vertex_render_data[], int start, SpriteSheet* spritesheet, Sprite* sprite, int frame)
{
	float x1, x2, y1, y2;

	if(spritesheet->type == SHEET_HORIZONTAL)
	{
		x1 = (float)(frame * sprite->sprite_width)/spritesheet->width;
		x2 = (float)((frame + 1) * sprite->sprite_width)/spritesheet->width;
		y1 = (float)(sprite->y * sprite->sprite_height)/spritesheet->height;
		y2 = (float)((sprite->y + 1) * sprite->sprite_height)/spritesheet->height;
	}
	if(spritesheet->type == SHEET_VERTICAL)
	{
		x1 = (float)(sprite->x * sprite->sprite_width)/spritesheet->width;
		x2 = (float)((sprite->x + 1) * sprite->sprite_width)/spritesheet->width;
		y1 = (float)(frame * sprite->sprite_height)/spritesheet->height;
		y2 = (float)((frame + 1) * sprite->sprite_height)/spritesheet->height;
	}
	
	(vertex_render_data)[start + 0].tex_coord = (vec2s){{x2, y2}};
	(vertex_render_data)[start + 1].tex_coord = (vec2s){{x2, y1}};
	(vertex_render_data)[start + 2].tex_coord = (vec2s){{x1, y1}};

	(vertex_render_data)[start + 3].tex_coord = (vec2s){{x1, y1}};
	(vertex_render_data)[start + 4].tex_coord = (vec2s){{x1, y2}};
	(vertex_render_data)[start + 5].tex_coord = (vec2s){{x2, y2}};
}

void get_spriteUV(UV_Coords* uv_coords, SpriteSheet* spritesheet, Sprite* sprite)
{
	uv_coords->x1 = (float)(sprite->x * sprite->sprite_width)/spritesheet->width;
	uv_coords->x2 = (float)((sprite->x + 1) * sprite->sprite_width)/spritesheet->width;

	uv_coords->y1 = (float)(sprite->y * sprite->sprite_height)/spritesheet->height;
	uv_coords->y2 = (float)((sprite->y + 1) * sprite->sprite_height)/spritesheet->height;
}

void destroy_spriteSheet()
{
	size_t i = 0;
    void* data;
    while(hashmap_iter(sprite_sheet_hashmap, &i, &data))
	{
		SpriteSheet* sheet = data;
		free(sheet->name);
		free(sheet->path);
		free(sheet->default_sprite);

		size_t j = 0;
		void* d;
		while(hashmap_iter(sheet->sprite_hashmap, &j, &d))
		{
			Sprite* sprite = d;
			free(sprite->name);
		}

		hashmap_free(sheet->sprite_hashmap);
    }

	hashmap_free(sprite_sheet_hashmap);
}
