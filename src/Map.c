#include "Map.h"
#include "VertexAttributes.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "SpriteSheet.h"
#include "Utils.h"

#if defined(_PLATFORM_DESKTOP)
#include <glad/gles2.h>
#elif defined(_PLATFORM_WEB)
#include <emscripten.h>
#include <GLES3/gl3.h>
#elif defined(_PLATFORM_ANDROID)
#include <GLES3/gl3.h>
#endif

#include <time.h>

#include <hashmap.h>
#include <stb_perlin.h>

#define MAP_SIZE_X 50
#define MAP_SIZE_Y 50
#define MAP_TILE_COUNT MAP_SIZE_X * MAP_SIZE_Y

static unsigned int shaderProgram;
static VertexAttributes vertexAttributes;

static Texture texture;
static SpriteSheet* spriteSheet;

static Quad tiles[MAP_TILE_COUNT];

static GLushort indices[MAP_TILE_COUNT * 6];
static vec3s tile_size = (vec3s){{0.02f, 0.03f}};

void fillMap()
{
	srand(time(NULL));
	const int seed = rand();

	// Vertices
	for(int y = 0; y < MAP_SIZE_Y; y++)
	{
		for(int x = 0; x < MAP_SIZE_X; x++)
		{
			float noise_x = (float)x/(float)MAP_SIZE_X;
			float noise_y = (float)y/(float)MAP_SIZE_Y;
			float noise_factor = 10.0f;
			static char* tileSprite = "grass";

			float perlin_noise = stb_perlin_noise3_seed(noise_x * noise_factor, noise_y * noise_factor, 0.0f, 0, 0, 0, seed); 
			
			if(perlin_noise < -0.2) tileSprite = "water";
			if(perlin_noise > -0.1) tileSprite = "grass";
			if(perlin_noise > 0.3) tileSprite = "stone";

			float x1 = (float)(x*texture.width*tile_size.x);
			float y1 = (float)(y*texture.height*tile_size.y);
			float x2 = (float)((x+1)*texture.width*tile_size.x);
			float y2 = (float)((y+1)*texture.height*tile_size.y);

			tiles[y * MAP_SIZE_X + x] = (Quad)
			{
				{
					{{{x2, y2, 0.0f}}, {{1.0f, 0.0f, 0.0f, 1.0f}}, {{1.0f, 1.0f}}},
					{{{x2, y1, 0.0f}}, {{1.0f, 0.0f, 0.0f, 1.0f}}, {{1.0f, 0.0f}}},
					{{{x1, y1, 0.0f}}, {{1.0f, 0.0f, 0.0f, 1.0f}}, {{0.0f, 0.0f}}},
					{{{x1, y2, 0.0f}}, {{1.0f, 0.0f, 0.0f, 1.0f}}, {{0.0f, 1.0f}}},
				}
			};

			Sprite* sprite = (Sprite*)hashmap_get(spriteSheet->spriteHashMap, &(Sprite){ .name=tileSprite });
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
	spriteSheet = (SpriteSheet*)hashmap_get(spriteSheetHashMap, &(SpriteSheet){ .name="tiles" });

	initTextureFromFile(&texture, spriteSheet->path);
	
	fillMap();
	initShaderProgram(&shaderProgram, "shaders/map-vertex-shader.glsl", "shaders/map-fragment-shader.glsl");
	initVertexAttributes(&vertexAttributes, tiles, sizeof(tiles), indices, sizeof(indices));
}

void drawMap(Camera* camera)
{
	bindShaderProgram(&shaderProgram);

	bindTexture(&texture);

	mat4s transform = GLMS_MAT4_IDENTITY_INIT;
	transform = glms_scale(transform, (vec3s){{1.0f, 1.0f, 0.0f}});
	transform = glms_translate(transform, (vec3s){{0.0f, 0.0f, 0.0f}});

	uniformMat4(&shaderProgram, "projection", camera->projection_matrix);
	uniformMat4(&shaderProgram, "view", camera->view_matrix);
	uniformMat4(&shaderProgram, "transform", transform);

	uniformInt(&shaderProgram, "textureSampler", 0);

	bindBuffer(&vertexAttributes, VAO);

	glDrawElements(GL_TRIANGLES, MAP_TILE_COUNT*6, GL_UNSIGNED_SHORT, 0);
}

void destroyMap()
{
	destroyTexture(&texture);
	destroyShaderProgram(&shaderProgram);
	destroyVertexAttributes(&vertexAttributes);
}
