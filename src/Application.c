#include "Application.h"

#define UTILS_DEF
#include "Utils.h"
#include "Window.h"
#include "Player.h"
#include "SpriteSheet.h"
#include "Map.h"

#if defined(_PLATFORM_NATIVE)
	#include <glad/gles2.h>
#elif defined(_PLATFORM_WEB)
	#include <emscripten.h>
	#include <GLES3/gl3.h>
#endif

#include <cglm/struct.h>

Window window;

mat4s projectionMatrix = GLMS_MAT4_IDENTITY_INIT;
mat4s viewMatrix = GLMS_MAT4_IDENTITY_INIT;
mat4s viewTimesProj = GLMS_MAT4_IDENTITY_INIT;

void init()
{
	window.width = WINDOW_WIDTH;
	window.height = WINDOW_HEIGHT;
	window.title = WINDOW_TITLE;

	initWindow(&window);
	changeWindowColor("#034694", 1.0f);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	initSpriteSheet();

	initPlayer();
	initMap();

	projectionMatrix = glms_ortho(0.0f, window.width, 0.0f, window.height, 0.001f, 1000.0f);
	viewMatrix = glms_translate(viewMatrix, (vec3s){{0.0f, 0.0f, -1.0f}});
	viewTimesProj = glms_mat4_mulN((mat4s*[]){&projectionMatrix, &viewMatrix}, 2);
}

void renderFrame()
{
	glClear(GL_COLOR_BUFFER_BIT);

	drawMap(viewTimesProj);
	drawPlayer(viewTimesProj);

	updateWindow(&window);
}

void cleanup()
{
	destroySpriteSheet();
	destroyMap();
	destroyPlayer();
	destroyWindow(&window);
}

void runApplication()
{
	init();
	
#if defined(_PLATFORM_NATIVE)
	while(!shouldWindowClose(&window))
	{
		renderFrame();
	}
#elif defined(_PLATFORM_WEB)
	emscripten_set_main_loop(renderFrame, 0, true);
#endif

	cleanup();
}
