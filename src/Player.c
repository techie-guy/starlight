#include "Player.h"
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


static VertexAttributes vertexAttributes;
static unsigned int shaderProgram;

static Texture texture;

static SpriteSheet* spriteSheet;
static char* playerSpriteName = "";

static Vertex vertices[4] =
{
	{{{1.0f, 1.0f, 0.0f}}, {{1.0f, 1.0f, 0.0f, 1.0f}}, {{0.16f, 1.0f}}},
	{{{1.0f, -1.0f, 0.0f}}, {{1.0f, 1.0f, 0.0f, 1.0f}}, {{0.16f, 0.0f}}},
	{{{-1.0f, -1.0f, 0.0f}}, {{1.0f, 1.0f, 0.0f, 1.0f}}, {{0.0f, 0.0f}}},
	{{{-1.0f, 1.0f, 0.0f}}, {{1.0f, 1.0f, 0.0f, 1.0f}}, {{0.0f, 1.0f}}},
};

static GLushort indices[6] =
{
	0, 1, 2,
	2, 3, 0
};

void initPlayer()
{
	spriteSheet = hashmap_get(spriteSheetHashMap, &(SpriteSheet){ .name="player" });

	playerSpriteName = spriteSheet->defaultSprite;

	initTexture(&texture, spriteSheet->path);
	initVertexAttributes(&vertexAttributes, vertices, sizeof(vertices), indices, sizeof(indices));
	initShaderProgram(&shaderProgram, "assets/shaders/player-vertex-shader.glsl", "assets/shaders/player-fragment-shader.glsl");
}

void updateTexCoords()
{
	static int currentFrame = 0;
	Sprite* sprite = hashmap_get(spriteSheet->spriteHashMap, &(Sprite){ .name=playerSpriteName });

	currentFrame++;

	int animFrame = (currentFrame/sprite->frameSpeed) % sprite->spriteCount;

	float x1 = (float)(animFrame * sprite->spriteWidth)/spriteSheet->width;
	float x2 = (float)((animFrame + 1) * sprite->spriteWidth)/spriteSheet->width;

	float y1 = (float)(sprite->y * sprite->spriteHeight)/spriteSheet->height;
	float y2 = (float)((sprite->y + 1) * sprite->spriteHeight)/spriteSheet->height;

	vertices[0].texCoord = (vec2s){{x2, y2}};
	vertices[1].texCoord = (vec2s){{x2, y1}};
	vertices[2].texCoord = (vec2s){{x1, y1}};
	vertices[3].texCoord = (vec2s){{x1, y2}};
}

void drawPlayer(const mat4s viewTimesProj)
{
	bindShaderProgram(&shaderProgram);

	bindTexture(&texture);

	mat4s transform = GLMS_MAT4_IDENTITY_INIT;
	transform = glms_scale(transform, (vec3s){{30.0f, 30.0f, 0.0f}});
	transform = glms_translate(transform, (vec3s){{5.0f, 5.0f, 0.0f}});

	mat4s mvp = glms_mat4_mulN((mat4s*[]){(mat4s*)&viewTimesProj, &transform}, 2);

	uniformMat4(&shaderProgram, "mvp", mvp);

	bindVAO(&vertexAttributes);
	bindVBO(&vertexAttributes);

	updateTexCoords();

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
}

void destroyPlayer()
{
	destroyTexture(&texture);
	destroyVertexAttributes(&vertexAttributes);
	destroyShaderProgram(&shaderProgram);
}
