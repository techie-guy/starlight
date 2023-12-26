#pragma once

#include "Utils.h"

typedef struct
{
	char* scene_name;
	void (*init)();
	void (*destroy)();
	void (*activate)();
	void (*deactivate)();
	void (*update)(float);
	void (*render)();
	void (*process_input)(InputState, float);
} Scene;

void addScene(Scene* scene);
void changeScene(char* scene_name);

void initScene();
void updateScene(float deltatime);
void renderScene();
void sceneProcessInput(InputState input_state, float deltatime);
void destroyScene();
