#pragma once

#include "../utils/utils.h"
#include <cglm/struct.h>

enum CameraType
{
	NO_MOVE = 1 << 0,
	LOOK_AROUND = 1 << 1,
	WALK_AROUND = 1 << 2,
};

typedef struct
{
	mat4s projection_matrix;
	mat4s view_matrix;

	vec3s position;
	vec3s target;
	vec3s up;

	vec3s front;

	float speed;
	float mouse_sensitivity;
	float fov;
	float near_plane;
	float far_plane;
	float aspect_ratio;

	// Rotation
	float pitch;
	float yaw;

	enum CameraType camera_type;
} Camera;

void init_camera(Camera* camera);
void update_camera(Camera* camera);
void move_camera(Camera* camera, InputState input_state, float deltatime);
