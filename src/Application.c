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

#if defined(_PLATFORM_DESKTOP)
	chdir("./assets");
#endif

	init_window(&window);
	change_window_color("#034694", 1.0f);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	init_spritesheet();
	init_font_renderer("fonts/font.ttf", 96, &window);

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
	
	ImFontAtlas_AddFontFromFileTTF(io->Fonts, "fonts/font.ttf", 20, NULL, ranges.Data);
	ImFontAtlas_Build(io->Fonts);

	ImVector_Destruct(&ranges);

	cImGui_ImplGlfw_InitForOpenGL(window.handle, true);
	cImGui_ImplOpenGL3_InitEx("#version 100");

	// Scene
	add_scene(&ScenePlay);
	change_scene("ScenePlay");
	init_scene(&window);
}

void render_frame()
{
	{
		current_frame = glfwGetTime();
		deltatime = current_frame - last_frame;
		last_frame = current_frame;
	}

	window_poll_events();

	update_scene(deltatime);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	cImGui_ImplOpenGL3_NewFrame();
	cImGui_ImplGlfw_NewFrame();
	ImGui_NewFrame();

	scene_process_input(window.input_system, deltatime);
	render_scene();

	ImGui_Render();

	cImGui_ImplOpenGL3_RenderDrawData(ImGui_GetDrawData());

	window_swap_buffers(&window);
}

void cleanup()
{
	destroy_ECS();
	destroy_scenes();

	destroy_spriteSheet();

	cImGui_ImplOpenGL3_Shutdown();
	cImGui_ImplGlfw_Shutdown();
	ImGui_DestroyContext(NULL);

	destroy_window(&window);
}

void run_application()
{
	init();
	
#if !defined(_PLATFORM_WEB)
	while(!should_window_close(&window))
	{
		render_frame();
	}
#else
	emscripten_set_main_loop(renderFrame, 0, true);
#endif

	cleanup();
}
