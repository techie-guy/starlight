#include "SpriteSheet.h"
#include "Utils.h"
#include <string.h>
#include <cJSON.h>
#include <hashmap.h>

struct hashmap* spriteSheetHashMap;

int spriteSheetCompare(const void* a, const void* b, void* udata)
{
	const SpriteSheet* ua = a;
	const SpriteSheet* ub = b;
	return strcmp(ua->name, ub->name);
}

int spriteCompare(const void* a, const void* b, void* udata)
{
	const Sprite* ua = a;
	const Sprite* ub = b;
	return strcmp(ua->name, ub->name);
}

uint64_t spriteSheetHash(const void* item, uint64_t seed0, uint64_t seed1)
{
	const SpriteSheet* sheet = item;
	return hashmap_sip(sheet->name, strlen(sheet->name), seed0, seed1);
}

uint64_t spriteHash(const void* item, uint64_t seed0, uint64_t seed1)
{
	const Sprite* sprite = item;
	return hashmap_sip(sprite->name, strlen(sprite->name), seed0, seed1);
}

void initSpriteSheet()
{
	spriteSheetHashMap = hashmap_new(sizeof(SpriteSheet), 0, 0, 0, spriteSheetHash, spriteSheetCompare, NULL, NULL);

	char* textureJsonString = readFile("assets/spritesheets.json");
	cJSON* textureJson = cJSON_Parse(textureJsonString);

	cJSON* spriteSheetsJson = cJSON_GetObjectItemCaseSensitive(textureJson, "spriteSheets");

	cJSON* spriteSheetJson;
	cJSON_ArrayForEach(spriteSheetJson, spriteSheetsJson)
	{
		char* sheetName = strdup(cJSON_GetObjectItemCaseSensitive(spriteSheetJson, "name")->valuestring);
		char* sheetPath = strdup(cJSON_GetObjectItemCaseSensitive(spriteSheetJson, "path")->valuestring);
		char* sheetDefaultSprite = strdup(cJSON_GetObjectItemCaseSensitive(spriteSheetJson, "defaultSprite")->valuestring);
		int sheetWidth = cJSON_GetObjectItemCaseSensitive(spriteSheetJson, "width")->valueint;
		int sheetHeight = cJSON_GetObjectItemCaseSensitive(spriteSheetJson, "height")->valueint;
		struct hashmap* spriteHashMap = hashmap_new(sizeof(Sprite), 0, 0, 0, spriteHash, spriteCompare, NULL, NULL);

		cJSON* spritesJson = cJSON_GetObjectItemCaseSensitive(spriteSheetJson, "sprites");

		cJSON* spriteJson;

		cJSON_ArrayForEach(spriteJson, spritesJson)
		{
			char* spriteName = strdup(cJSON_GetObjectItemCaseSensitive(spriteJson, "name")->valuestring);
			int spriteX = cJSON_GetObjectItemCaseSensitive(spriteJson, "x")->valueint;
			int spriteY = cJSON_GetObjectItemCaseSensitive(spriteJson, "y")->valueint;
			int spriteFrameSpeed = cJSON_GetObjectItemCaseSensitive(spriteJson, "frameSpeed")->valueint;
			int spriteCount = cJSON_GetObjectItemCaseSensitive(spriteJson, "spriteCount")->valueint;
			int spriteWidth = cJSON_GetObjectItemCaseSensitive(spriteJson, "spriteWidth")->valueint;
			int spriteHeight = cJSON_GetObjectItemCaseSensitive(spriteJson, "spriteHeight")->valueint;	

			hashmap_set(spriteHashMap, &(Sprite){
				.name=spriteName,
				.x=spriteX, .y=spriteY,
				.frameSpeed=spriteFrameSpeed,
				.spriteCount=spriteCount,
				.spriteWidth=spriteWidth, .spriteHeight=spriteHeight,
			});
		}

		hashmap_set(spriteSheetHashMap, &(SpriteSheet){
			.name=sheetName,
			.path=sheetPath,
			.defaultSprite=sheetDefaultSprite,
			.width=sheetWidth, .height=sheetHeight,
			.spriteHashMap=spriteHashMap
		});
	}

	cJSON_Delete(textureJson);
	free(textureJsonString);
}

void getSpriteAnimation(Quad* quad, SpriteSheet* spriteSheet, Sprite* sprite, int frame)
{
	float x1 = (float)(frame * sprite->spriteWidth)/spriteSheet->width;
	float x2 = (float)((frame + 1) * sprite->spriteWidth)/spriteSheet->width;

	float y1 = (float)(sprite->y * sprite->spriteHeight)/spriteSheet->height;
	float y2 = (float)((sprite->y + 1) * sprite->spriteHeight)/spriteSheet->height;

	quad->vertices[0].texCoord = (vec2s){{x2, y2}};
	quad->vertices[1].texCoord = (vec2s){{x2, y1}};
	quad->vertices[2].texCoord = (vec2s){{x1, y1}};
	quad->vertices[3].texCoord = (vec2s){{x1, y2}};
}

void getSpriteUV(Quad* quad, SpriteSheet* spriteSheet, Sprite* sprite)
{
	float x1 = (float)(sprite->x * sprite->spriteWidth)/spriteSheet->width;
	float x2 = (float)((sprite->x + 1) * sprite->spriteWidth)/spriteSheet->width;

	float y1 = (float)(sprite->y * sprite->spriteHeight)/spriteSheet->height;
	float y2 = (float)((sprite->y + 1) * sprite->spriteHeight)/spriteSheet->height;

	quad->vertices[0].texCoord = (vec2s){{x2, y2}};
	quad->vertices[1].texCoord = (vec2s){{x2, y1}};
	quad->vertices[2].texCoord = (vec2s){{x1, y1}};
	quad->vertices[3].texCoord = (vec2s){{x1, y2}};
}

void destroySpriteSheet()
{
	size_t i = 0;
    void* data;
    while(hashmap_iter(spriteSheetHashMap, &i, &data))
	{
		SpriteSheet* sheet = data;
		free(sheet->name);
		free(sheet->path);
		free(sheet->defaultSprite);

		size_t j = 0;
		void* d;
		while(hashmap_iter(sheet->spriteHashMap, &j, &d))
		{
			Sprite* sprite = d;
			free(sprite->name);
		}

		hashmap_free(sheet->spriteHashMap);
    }

	hashmap_free(spriteSheetHashMap);
}
