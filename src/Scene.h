#pragma once

typedef struct
{
	char* scene_name;
	void (*init)();
	void (*destroy)();
	void (*activate)();
	void (*deactivate)();
	void (*update)();
} Scene;

void addScene(Scene* scene);
void changeScene(char* scene_name);

void initScene();
void updateScene();
void destroyScene();
