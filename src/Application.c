#include "Application.h"
#include "FontRenderer.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "cimgui.h"
#include "cimgui_impl_glfw.h"
#include "cimgui_impl_opengl3.h"

#define UTILS_DEF
#include "Utils.h"
#include "Window.h"
#include "Player.h"
#include "SpriteSheet.h"
#include "Map.h"
#include "Camera.h"
#include "ECS.h"

#if defined(_PLATFORM_NATIVE)
	#include <glad/gles2.h>
#elif defined(_PLATFORM_WEB)
	#include <emscripten.h>
	#include <GLES3/gl3.h>
#endif

#include <cglm/struct.h>

Window window;
Camera camera;

InputState inputState;

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
		if(key == GLFW_KEY_SPACE) inputState.space = true;
		if(key == GLFW_KEY_LEFT_CONTROL) inputState.l_ctrl = true;
	}
	if(action == GLFW_RELEASE)
	{
		inputState.keyPress = false;

		if(key == GLFW_KEY_UP || key == GLFW_KEY_W) inputState.up = false;
		if(key == GLFW_KEY_DOWN || key == GLFW_KEY_S) inputState.down = false;
		if(key == GLFW_KEY_LEFT || key == GLFW_KEY_A) inputState.left = false;
		if(key == GLFW_KEY_RIGHT || key == GLFW_KEY_D) inputState.right = false;
		if(key == GLFW_KEY_SPACE) inputState.space = false;
		if(key == GLFW_KEY_LEFT_CONTROL) inputState.l_ctrl = false;
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

	initFontRenderer("assets/fonts/font.ttf", 48);

	camera.window_dimensions = (vec2s){{ WINDOW_WIDTH, WINDOW_HEIGHT }};
	camera.position = (vec3s){{5.0f, 5.0f, -10.0f}};
	camera.target = (vec3s){{0.0f, 0.0f, 0.0f}};
	camera.up = (vec3s){{0.0f, 1.0f, 0.0f}};
	camera.speed = 5.0f;
	initCamera(&camera);


	ImGui_CreateContext(NULL);
    ImGuiIO* io = ImGui_GetIO();

	cImGui_ImplGlfw_InitForOpenGL(window.handle, true);
	cImGui_ImplOpenGL3_InitEx("#version 100");	
}

void handleInput()
{
	updatePlayer(inputState, deltaTime);
	moveCamera(&camera, inputState, deltaTime);
}

void renderFrame()
{
	{
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
	}

	windowPollEvents();

	cImGui_ImplOpenGL3_NewFrame();
	cImGui_ImplGlfw_NewFrame();
	ImGui_NewFrame();

	ImGui_ShowDemoWindow(NULL);

	ImGui_Render();

	handleInput();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	{
		updateCamera(&camera);
		viewTimesProj = glms_mat4_mulN((mat4s*[]){&camera.projection_matrix, &camera.view_matrix}, 2);
	}

	drawMap(viewTimesProj);
	drawPlayer(viewTimesProj);

	renderText("Hello", 25.0f, 25.0f, 1.0f, (vec3s){1.0f, 1.0f, 1.0f});

	cImGui_ImplOpenGL3_RenderDrawData(ImGui_GetDrawData());

	windowSwapBuffers(&window);
}

void cleanup()
{
	destroyECS();

	cImGui_ImplOpenGL3_Shutdown();
	cImGui_ImplGlfw_Shutdown();
	ImGui_DestroyContext(NULL);

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
