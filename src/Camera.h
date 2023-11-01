#pragma once

#include "Utils.h"
#include <cglm/struct.h>

typedef struct
{
	vec2s window_dimensions;
	mat4s projection_matrix;
	mat4s view_matrix;

	vec3s position;
	vec3s target;
	vec3s up;

	float speed;
} Camera;

void initCamera(Camera* camera);
void updateCamera(Camera* camera);
void moveCamera(Camera* camera, InputState inputState, float deltaTime);
