#include "Scene.h"

#include <stb_ds.h>

static struct { char* key; Scene* value; }* scenes = NULL;

static Scene* current_scene;
static unsigned int total_scenes;

void add_scene(Scene* scene)
{
	shput(scenes, scene->scene_name, scene);
	total_scenes++;
}

void change_scene(char* scene_name)
{
	Scene* s = shget(scenes, scene_name);

	if(current_scene)
	{
		current_scene->deactivate();
	}

	current_scene = s;

	current_scene->activate();
}

void init_scene(Window* window)
{
	current_scene->init(window);
}

void update_scene(float deltatime)
{
	current_scene->update(deltatime);
}

void render_scene()
{
	current_scene->render();
}

void scene_process_input(InputSystem input_system, float deltatime)
{
	current_scene->process_input(input_system, deltatime);
}

void destroy_scene()
{
	current_scene->destroy();
}

void destroy_scenes()
{
	for(int i = 0; i < shlen(scenes); i++)
	{
		scenes[i].value->destroy();
	}
}
