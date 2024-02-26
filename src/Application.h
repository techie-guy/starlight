#pragma once

#include "Window.h"
#include "Scene.h"

typedef struct
{
	Window current_window;
	Scene* current_scene;
	float deltatime;
} GameEngine;

extern GameEngine game_engine;

void run_application();
