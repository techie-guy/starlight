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

void add_scene(Scene* scene);
void change_scene(char* scene_name);

void init_scene();
void update_scene(float deltatime);
void render_scene();
void scene_process_input(InputSystem input_system, float deltatime);
void destroy_scene();
void destroy_scenes();
