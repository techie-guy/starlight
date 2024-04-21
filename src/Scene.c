#include "Scene.h"
#include "Application.h"

#include <stb_ds.h>

static struct { char* key; Scene* value; }* used_scenes = NULL;

static unsigned int total_scenes;

void add_scene(Scene* scene)
{
	shput(game_engine.scenes, scene->scene_name, scene);
	total_scenes++;
}

void change_scene(char* scene_name)
{
	Scene* scene = shget(game_engine.scenes, scene_name);
	shput(used_scenes, scene_name, scene);

	if(game_engine.current_scene)
	{
		game_engine.current_scene->deactivate();
	}

	game_engine.current_scene = scene;

	game_engine.current_scene->init();
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
	for(int i = 0; i < shlen(used_scenes); i++)
	{
		used_scenes[i].value->destroy();
	}
}
