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

void sceneProcessInput(InputState input_state, float deltatime)
{
	current_scene->process_input(input_state, deltatime);
}

void destroyScene()
{
	current_scene->destroy();
}
