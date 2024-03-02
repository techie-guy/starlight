#include "Application.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Utils.h"
#include "Window.h"
#include "SpriteSheet.h"
#include "FontRenderer.h"
#include "ECS.h"
#include "Scene.h"
#include "ScenePlay.h"
#include "SceneBlock.h"
#include "UI-Imgui.h"

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

// Delta Time
static float current_frame = 0.0f;
static float last_frame = 0.0f;

GameEngine game_engine = (GameEngine){};

static void init()
{
	game_engine.current_window.title = WINDOW_TITLE;
	game_engine.current_window.width = WINDOW_WIDTH;
	game_engine.current_window.height = WINDOW_HEIGHT;

#if !defined(_PLATFORM_ANDROID)
	chdir(ASSETS_DIR);
#endif

	init_window(&game_engine.current_window);
	change_window_color(hex_to_rbg("#333333", 1.0f));
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	init_spritesheet();
	init_font_renderer("fonts/font.ttf", 96);
	init_imgui("fonts/font.ttf", 20, "", "#version 100");

	// Scene
	add_scene(&ScenePlay);
	add_scene(&SceneBlock);
	change_scene("SceneBlock");
}

static void render_frame()
{
	{
		current_frame = glfwGetTime();
		game_engine.deltatime = current_frame - last_frame;
		last_frame = current_frame;
	}

	window_poll_events();

	update_scene();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	new_imgui_frame();
	
	scene_process_input();
	render_scene();

	render_imgui();
	
	window_swap_buffers(&game_engine.current_window);
}

static void cleanup()
{
	destroy_ECS();
	destroy_scenes();

	destroy_spriteSheet();
	
	destroy_imgui();
	destroy_window(&game_engine.current_window);
}

void run_application()
{
	init();
	
#if !defined(_PLATFORM_WEB)
	while(!should_window_close(&game_engine.current_window))
	{
		render_frame();
	}
#else
	emscripten_set_main_loop(render_frame, 0, true);
#endif

	cleanup();
}
