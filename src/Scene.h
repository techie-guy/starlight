#pragma once

#include "Utils.h"
#include "Window.h"

typedef struct
{
	char* scene_name;
	void (*init)(Window*);
	void (*destroy)();
	void (*activate)();
	void (*deactivate)();
	void (*update)(float);
	void (*render)();
	void (*process_input)(InputSystem, float);
} Scene;

void addScene(Scene* scene);
void changeScene(char* scene_name);

void initScene();
void updateScene(float deltatime);
void renderScene();
void sceneProcessInput(InputSystem input_system, float deltatime);
void destroyScene();
void destroyScenes();
