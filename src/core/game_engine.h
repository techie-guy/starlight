#pragma once

#include "window.h"
#include "scene.h"

typedef struct
{
	Window current_window;
	Scene* current_scene;
	float deltatime;
	struct { char* key; Scene* value; }* scenes;
} GameEngine;

extern GameEngine game_engine;

void run_application();
