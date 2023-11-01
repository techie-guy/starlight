#pragma once

#include <cglm/struct.h>
#include "Utils.h"

void initPlayer(); 
void updatePlayer(InputState inputState, float deltaTime);
void drawPlayer(const mat4s viewTimesProj);
void destroyPlayer();
