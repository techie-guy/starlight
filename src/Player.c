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
static char* currentSpriteName = "";

static Quad quad[1] =
{
	(Quad)
	{
		{
		{{{1.0f, 1.0f, 0.0f}}, {{0.0f, 0.0f, 0.0f, 0.0f}}, {{1.0f, 1.0f}}},
		{{{1.0f, -1.0f, 0.0f}}, {{0.0f, 0.0f, 0.0f, 0.0f}}, {{1.0f, 0.0f}}},
		{{{-1.0f, -1.0f, 0.0f}}, {{0.0f, 0.0f, 0.0f, 0.0f}}, {{0.0f, 0.0f}}},
		{{{-1.0f, 1.0f, 0.0f}}, {{0.0f, 0.0f, 0.0f, 0.0f}}, {{0.0f, 1.0f}}},
		}
	}
};

static GLushort indices[6] =
{
	0, 1, 2,
	2, 3, 0
};

static vec3s position;
static vec3s scale;
static float speed = 4.0f;

void initPlayer()
{
	spriteSheet = hashmap_get(spriteSheetHashMap, &(SpriteSheet){ .name="player" });

	currentSpriteName = spriteSheet->defaultSprite;

	initTexture(&texture, spriteSheet->path);
	initVertexAttributes(&vertexAttributes, quad, sizeof(quad), indices, sizeof(indices));
	initShaderProgram(&shaderProgram, "assets/shaders/player-vertex-shader.glsl", "assets/shaders/player-fragment-shader.glsl");

	position = (vec3s){{5.0f, 5.0f, 0.0f}};
	scale = (vec3s){{30.0f, 30.0f, 0.0f}};
}

void updatePlayer(InputState inputState, float deltaTime)
{
	if(!inputState.keyPress)
	{
		currentSpriteName = spriteSheet->defaultSprite;
	}

	if(inputState.up)
	{
		position.y += speed * deltaTime;
		currentSpriteName = "move-up";
	}
	if(inputState.down)
	{
		position.y -= speed * deltaTime;
		currentSpriteName = "move-down";
	}
	if(inputState.left)
	{
		position.x -= speed * deltaTime;
		currentSpriteName = "strafe";
	}
	if(inputState.right)
	{
		position.x += speed * deltaTime;
		currentSpriteName = "strafe";
	}
}

void updateTexCoords()
{
	static int currentFrame = 0;
	Sprite* sprite = hashmap_get(spriteSheet->spriteHashMap, &(Sprite){ .name=currentSpriteName });

	currentFrame++;

	int animFrame = (currentFrame/sprite->frameSpeed) % sprite->spriteCount;

	getSpriteAnimation(quad, spriteSheet, sprite, animFrame);
}

void drawPlayer(const mat4s viewTimesProj)
{
	bindShaderProgram(&shaderProgram);

	bindTexture(&texture);

	mat4s transform = GLMS_MAT4_IDENTITY_INIT;
	transform = glms_scale(transform, scale);
	transform = glms_translate(transform, position);

	mat4s mvp = glms_mat4_mulN((mat4s*[]){(mat4s*)&viewTimesProj, &transform}, 2);

	uniformMat4(&shaderProgram, "mvp", mvp);

	bindVAO(&vertexAttributes);
	bindVBO(&vertexAttributes);

	updateTexCoords();

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad), quad);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
}

void destroyPlayer()
{
	destroyTexture(&texture);
	destroyVertexAttributes(&vertexAttributes);
	destroyShaderProgram(&shaderProgram);
}
