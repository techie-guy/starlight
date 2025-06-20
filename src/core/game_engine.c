#include "game_engine.h"
#include "window.h"
#include "font_renderer.h"
#include "ecs.h"

#include "../utils/utils.h"
#include "../utils/ui_imgui.h"
#include "../utils/spritesheet.h"

#include "../network/network.h"

#include "scene.h"
#include "../scenes/scenes.h"


#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


#if defined(_PLATFORM_DESKTOP)
	#include <glad/gles2.h>
#elif defined(_PLATFORM_WEB)
	#include <emscripten.h>
	#include <emscripten/html5.h> 
	#include <GLES3/gl3.h>
#elif defined(_PLATFORM_ANDROID)
	#include <GLES3/gl3.h>
	#include <android/native_window.h>
	#define GLFW_EXPOSE_NATIVE_ANDROID
	#include <GLFW/glfw3native.h>
#endif


// Delta Time
static float current_frame = 0.0f;
static float last_frame = 0.0f;

GameEngine game_engine = (GameEngine){};

static void init()
{
	game_engine.current_window.title = WINDOW_TITLE;

#if defined(_PLATFORM_WEB)
	game_engine.current_window.width = 1920;
	game_engine.current_window.height = 1080;

	emscripten_request_fullscreen_strategy("canvas", true, &(EmscriptenFullscreenStrategy)
	{
		.scaleMode=EMSCRIPTEN_FULLSCREEN_SCALE_DEFAULT,
		.canvasResolutionScaleMode=EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_NONE,
		.filteringMode=EMSCRIPTEN_FULLSCREEN_FILTERING_NEAREST
	});
#else
	game_engine.current_window.width = WINDOW_WIDTH;
	game_engine.current_window.height = WINDOW_HEIGHT;
#endif

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
	init_network();
//	ImGui_GetIO()->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	// Scene
	load_scenes();
	change_scene("SceneMenu");
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
	destroy_network();

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
