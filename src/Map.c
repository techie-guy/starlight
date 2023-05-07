#include "Map.h"
#include "VertexAttributes.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "SpriteSheet.h"
#include "Utils.h"

#if defined(_PLATFORM_NATIVE)
	#include <glad/gles2.h>
#elif defined(_PLATFORM_WEB)
	#include <GLES3/gl3.h>
#endif

#include <hashmap.h>

#define MAP_SIZE_X 30
#define MAP_SIZE_Y 30
#define MAP_TILE_COUNT MAP_SIZE_X * MAP_SIZE_Y

static unsigned int shaderProgram;
static VertexAttributes vertexAttributes;

static Texture texture;
static SpriteSheet* spriteSheet;

static Quad tiles[MAP_TILE_COUNT];

static GLushort indices[MAP_TILE_COUNT * 6];

void fillMap()
{
	// Vertices
	for(int y = 0; y < MAP_SIZE_Y; y++)
	{
		for(int x = 0; x < MAP_SIZE_X; x++)
		{
			float x1 = (float)(x*texture.width);
			float y1 = (float)(y*texture.height);
			float x2 = (float)((x+1)*texture.width);
			float y2 = (float)((y+1)*texture.height);

			tiles[y * MAP_SIZE_X + x] = (Quad)
			{
				{
					{{{x2, y2, 0.0f}}, {{1.0f, 0.0f, 0.0f, 1.0f}}, {{1.0f, 1.0f}}},
					{{{x2, y1, 0.0f}}, {{1.0f, 0.0f, 0.0f, 1.0f}}, {{1.0f, 0.0f}}},
					{{{x1, y1, 0.0f}}, {{1.0f, 0.0f, 0.0f, 1.0f}}, {{0.0f, 0.0f}}},
					{{{x1, y2, 0.0f}}, {{1.0f, 0.0f, 0.0f, 1.0f}}, {{0.0f, 1.0f}}},
				}
			};

			Sprite* sprite = hashmap_get(spriteSheet->spriteHashMap, &(Sprite){ .name="stone" });
			getSpriteUV(&tiles[y * MAP_SIZE_X + x], spriteSheet, sprite);
		}
	}

	// Indices
	for(int i = 0; i < sizeof(indices)/sizeof(indices[0]); i += 6)
	{
		static int offset = 0;

		indices[i + 0] = 0 + offset;
		indices[i + 1] = 1 + offset;
		indices[i + 2] = 2 + offset;
		
		indices[i + 3] = 2 + offset;
		indices[i + 4] = 3 + offset;
		indices[i + 5] = 0 + offset;

		offset += 4;
	}
}

void initMap()
{
	spriteSheet = hashmap_get(spriteSheetHashMap, &(SpriteSheet){ .name="tiles" });

	initTexture(&texture, spriteSheet->path);
	
	fillMap();
	initShaderProgram(&shaderProgram, "assets/shaders/map-vertex-shader.glsl", "assets/shaders/map-fragment-shader.glsl");
	initVertexAttributes(&vertexAttributes, tiles, sizeof(tiles), indices, sizeof(indices));
}

void drawMap(const mat4s viewTimesProj)
{
	bindShaderProgram(&shaderProgram);

	bindTexture(&texture);

	mat4s transform = GLMS_MAT4_IDENTITY_INIT;
	transform = glms_scale(transform, (vec3s){{0.5f, 2.0f, 0.0f}});
	transform = glms_translate(transform, (vec3s){{0.0f, 0.0f, 0.0f}});

	mat4s mvp = glms_mat4_mulN((mat4s*[]){(mat4s*)&viewTimesProj, &transform}, 2);

	uniformInt(&shaderProgram, "textureSampler", 0);
	uniformMat4(&shaderProgram, "mvp", mvp);

	bindVAO(&vertexAttributes);

	glDrawElements(GL_TRIANGLES, MAP_TILE_COUNT*6, GL_UNSIGNED_SHORT, 0);
}

void destroyMap()
{
	destroyTexture(&texture);

	destroyShaderProgram(&shaderProgram);
	destroyVertexAttributes(&vertexAttributes);
}
