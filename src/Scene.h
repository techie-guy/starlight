#pragma once

#include "Utils.h"

typedef struct
{
	char* scene_name;

	void (*init)();
	void (*destroy)();
	void (*activate)();
	void (*deactivate)();
	void (*update)();
	void (*render)();
	void (*process_input)();
} Scene;

void add_scene(Scene* scene);
void change_scene(char* scene_name);

void update_scene();
void render_scene();
void scene_process_input();
void destroy_scene();
void destroy_scenes();
