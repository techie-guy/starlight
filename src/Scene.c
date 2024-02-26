#include "Scene.h"
#include "Application.h"

#include <stb_ds.h>

static struct { char* key; Scene* value; }* scenes = NULL;

static unsigned int total_scenes;

void add_scene(Scene* scene)
{
	shput(scenes, scene->scene_name, scene);
	total_scenes++;

	scene->init();
}

void change_scene(char* scene_name)
{
	Scene* s = shget(scenes, scene_name);

	if(game_engine.current_scene)
	{
		game_engine.current_scene->deactivate();
	}

	game_engine.current_scene = s;

	game_engine.current_scene->activate();
}

void update_scene()
{
	game_engine.current_scene->update();
}

void render_scene()
{
	game_engine.current_scene->render();
}

void scene_process_input()
{
	game_engine.current_scene->process_input();
}

void destroy_scene()
{
	game_engine.current_scene->destroy();
}

void destroy_scenes()
{
	for(int i = 0; i < shlen(scenes); i++)
	{
		scenes[i].value->destroy();
	}
}
