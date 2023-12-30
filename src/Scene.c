#include "Scene.h"

#include <stb_ds.h>

static struct { char* key; Scene* value; }* scenes = NULL;

static Scene* current_scene;
static unsigned int total_scenes;

void addScene(Scene* scene)
{
	shput(scenes, scene->scene_name, scene);
	total_scenes++;
}

void changeScene(char* scene_name)
{
	Scene* s = shget(scenes, scene_name);

	if(current_scene)
	{
		current_scene->deactivate();
	}

	current_scene = s;

	current_scene->activate();
}

void initScene()
{
	current_scene->init();
}

void updateScene(float deltatime)
{
	current_scene->update(deltatime);
}

void renderScene()
{
	current_scene->render();
}

void sceneProcessInput(InputSystem input_system, float deltatime)
{
	current_scene->process_input(input_system, deltatime);
}

void destroyScene()
{
	current_scene->destroy();
}

void destroyScenes()
{
	for(int i = 0; i < shlen(scenes); i++)
	{
		scenes[i].value->destroy();
	}
}
