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

void updateScene()
{
	current_scene->update();
}

void destroyScene()
{
	current_scene->destroy();
}
