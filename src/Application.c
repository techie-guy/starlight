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

InputState inputState;

mat4s projectionMatrix = GLMS_MAT4_IDENTITY_INIT;
mat4s viewMatrix = GLMS_MAT4_IDENTITY_INIT;
mat4s viewTimesProj = GLMS_MAT4_IDENTITY_INIT;

// Delta Time
float deltaTime = 0.0f;
float currentFrame = 0.0f;
float lastFrame = 0.0f;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(action == GLFW_PRESS)
	{
		inputState.keyPress = true;

		if(key == GLFW_KEY_UP || key == GLFW_KEY_W) inputState.up = true;
		if(key == GLFW_KEY_DOWN || key == GLFW_KEY_S) inputState.down = true;
		if(key == GLFW_KEY_LEFT || key == GLFW_KEY_A) inputState.left = true;
		if(key == GLFW_KEY_RIGHT || key == GLFW_KEY_D) inputState.right = true;
	}
	if(action == GLFW_RELEASE)
	{
		inputState.keyPress = false;

		if(key == GLFW_KEY_UP || key == GLFW_KEY_W) inputState.up = false;
		if(key == GLFW_KEY_DOWN || key == GLFW_KEY_S) inputState.down = false;
		if(key == GLFW_KEY_LEFT || key == GLFW_KEY_A) inputState.left = false;
		if(key == GLFW_KEY_RIGHT || key == GLFW_KEY_D) inputState.right = false;
	}
}

void init()
{
	window.width = WINDOW_WIDTH;
	window.height = WINDOW_HEIGHT;
	window.title = WINDOW_TITLE;

	initWindow(&window);
	changeWindowColor("#034694", 1.0f);
	setWindowKeyCallback(&window, keyCallback);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	initSpriteSheet();

	initPlayer();
	initMap();

	projectionMatrix = glms_ortho(0.0f, window.width, 0.0f, window.height, 0.001f, 1000.0f);
	viewMatrix = glms_translate(viewMatrix, (vec3s){{0.0f, 0.0f, -1.0f}});
	viewTimesProj = glms_mat4_mulN((mat4s*[]){&projectionMatrix, &viewMatrix}, 2);
}

void handleInput()
{
	updatePlayer(inputState, deltaTime);
}

void renderFrame()
{
	{
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
	}

	handleInput();

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
