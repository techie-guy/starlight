#include "Camera.h"
#include "cglm/struct/cam.h"
#include "cglm/struct/vec3.h"

vec3s camera_front;
vec3s camera_right;
vec3s camera_up;

void init_camera(Camera* camera)
{
	camera->projection_matrix = (mat4s)GLMS_MAT4_IDENTITY_INIT;
	camera->view_matrix = (mat4s)GLMS_MAT4_IDENTITY_INIT;
}

void update_camera(Camera* camera)
{
	camera_front = (vec3s){{0.0f, 0.0f, 1.0f}};
	camera_right = glms_vec3_normalize(glms_vec3_cross(camera_front, camera->up));
	camera_up = glms_vec3_normalize(glms_vec3_cross(camera_right, camera_front));

	// Make this customisable
//	camera->projection_matrix = glms_ortho(0.0f, camera->window_dimensions.x, 0.0f, camera->window_dimensions.y, 0.001f, 1000.0f);
	camera->projection_matrix = glms_perspective(glm_rad(45.0f), (float)(camera->window_dimensions.x/camera->window_dimensions.y), 0.1f, 100.0f);
//	camera->view_matrix = glms_translate(camera->view_matrix, (vec3s){{0.0f, 0.0f, -1.0f}});
//	camera->view_matrix = glms_translate(camera->view_matrix, camera->position);
	camera->view_matrix = glms_lookat(camera->position, glms_vec3_add(camera->position, camera_front), camera_up);
}

void move_camera(Camera* camera, InputState input_state, float deltatime)
{	
	if(input_state.up)
	{
		camera->position.z += camera->speed * deltatime;
	}
	if(input_state.down)
	{
		camera->position.z -= camera->speed * deltatime;
	}
	if(input_state.left)
	{
		camera->position.x += camera->speed * deltatime;
	}
	if(input_state.right)
	{
		camera->position.x -= camera->speed * deltatime;
	}
	if(input_state.space)
	{
		camera->position.y += camera->speed * deltatime;
	}
	if(input_state.l_ctrl)
	{
		camera->position.y -= camera->speed * deltatime;
	}
}
