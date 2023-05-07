#pragma once

#include <cglm/struct.h>

typedef struct
{
	bool left;
	bool right;
	bool up;
	bool down;
	bool keyPress;
} InputState;

void initPlayer(); 
void updatePlayer(InputState inputState, float deltaTime);
void drawPlayer(const mat4s viewTimesProj);
void destroyPlayer();
