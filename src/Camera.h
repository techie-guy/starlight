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

void init_camera(Camera* camera);
void update_camera(Camera* camera);
void move_camera(Camera* camera, InputState input_state, float deltatime);
