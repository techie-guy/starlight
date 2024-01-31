#include "Application.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "cimgui.h"
#include "cimgui_impl_glfw.h"
#include "cimgui_impl_opengl3.h"

#define UTILS_DEF
#include "Utils.h"
#include "Window.h"
#include "SpriteSheet.h"
#include "FontRenderer.h"
#include "ECS.h"
#include "Scene.h"
#include "ScenePlay.h"

#if defined(_PLATFORM_DESKTOP)
	#include <glad/gles2.h>
#elif defined(_PLATFORM_WEB)
	#include <emscripten.h>
	#include <GLES3/gl3.h>
#elif defined(_PLATFORM_ANDROID)
	#include <GLES3/gl3.h>
	#include <android/native_window.h>
	#define GLFW_EXPOSE_NATIVE_ANDROID
	#include <GLFW/glfw3native.h>
#endif

#include <cglm/struct.h>

Window window;

// Delta Time
float deltatime = 0.0f;
float current_frame = 0.0f;
float last_frame = 0.0f;

void init()
{
	window.title = WINDOW_TITLE;
	window.width = WINDOW_WIDTH;
	window.height = WINDOW_HEIGHT;

	initWindow(&window);
	changeWindowColor("#034694", 1.0f);

	addScene(&ScenePlay);

	changeScene("ScenePlay");

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	initSpriteSheet();
	initFontRenderer("assets/fonts/font.ttf", 96, &window);

	initScene(&window);

	// Init ImGui
	ImGui_CreateContext(NULL);
    ImGuiIO* io = ImGui_GetIO();

	io->IniFilename = "/data/data/io.github.techieguy.mistlib/files/imgui.ini";

	ImVector_ImWchar ranges;
	ImVector_Construct(&ranges);
	ImFontGlyphRangesBuilder builder;
	memset(&builder, 0, sizeof(builder));

	ImFontGlyphRangesBuilder_Clear(&builder);
	ImFontGlyphRangesBuilder_AddRanges(&builder, ImFontAtlas_GetGlyphRangesDefault(io->Fonts));
	ImFontGlyphRangesBuilder_AddText(&builder, "", NULL);
	ImFontGlyphRangesBuilder_BuildRanges(&builder, &ranges);
	
	ImFontAtlas_AddFontFromFileTTF(io->Fonts, "assets/fonts/font.ttf", 20, NULL, ranges.Data);
	ImFontAtlas_Build(io->Fonts);

	ImVector_Destruct(&ranges);

	cImGui_ImplGlfw_InitForOpenGL(window.handle, true);
	cImGui_ImplOpenGL3_InitEx("#version 100");
}

void renderFrame()
{
	{
		current_frame = glfwGetTime();
		deltatime = current_frame - last_frame;
		last_frame = current_frame;
	}

	windowPollEvents();

	updateScene(deltatime);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	cImGui_ImplOpenGL3_NewFrame();
	cImGui_ImplGlfw_NewFrame();
	ImGui_NewFrame();

	sceneProcessInput(window.input_system, deltatime);
	renderScene();

	ImGui_Render();

	cImGui_ImplOpenGL3_RenderDrawData(ImGui_GetDrawData());

	windowSwapBuffers(&window);
}

void cleanup()
{
	destroyECS();
	destroyScenes();

	destroySpriteSheet();

	cImGui_ImplOpenGL3_Shutdown();
	cImGui_ImplGlfw_Shutdown();
	ImGui_DestroyContext(NULL);

	destroyWindow(&window);
}

void runApplication()
{
	init();
	
#if !defined(_PLATFORM_WEB)
	while(!shouldWindowClose(&window))
	{
		renderFrame();
	}
#else
	emscripten_set_main_loop(renderFrame, 0, true);
#endif

	cleanup();
}
