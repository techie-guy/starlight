#include "Camera.h"
#include "Application.h"

#include <GLFW/glfw3.h>
#include <cglm/struct.h>

static vec3s camera_direction;
static float last_x, last_y;
static bool first_mouse = true;

void init_camera(Camera* camera)
{
	last_x = game_engine.current_window.width/2.0f;
	last_y = game_engine.current_window.height/2.0f;

	camera->projection_matrix = (mat4s)GLMS_MAT4_IDENTITY_INIT;
	camera->view_matrix = (mat4s)GLMS_MAT4_IDENTITY_INIT;
}

void update_camera(Camera* camera)
{
	camera->aspect_ratio = (float)game_engine.current_window.width/game_engine.current_window.height;
	// Make this customisable
//	camera->projection_matrix = glms_ortho(0.0f, camera->window_dimensions.x, 0.0f, camera->window_dimensions.y, 0.001f, 1000.0f);
	camera->projection_matrix = glms_perspective(camera->fov, camera->aspect_ratio, camera->near_plane, camera->far_plane);
//	camera->view_matrix = glms_translate(camera->view_matrix, (vec3s){{0.0f, 0.0f, -1.0f}});
//	camera->view_matrix = glms_translate(camera->view_matrix, camera->position);
	camera->view_matrix = glms_lookat(camera->position, glms_vec3_add(camera->position, camera->front), camera->up);

}

void move_camera(Camera* camera, InputState input_state, float deltatime)
{
	if(camera->camera_type & WALK_AROUND)
	{
		if(input_state.up)
			camera->position = glms_vec3_add(camera->position, glms_vec3_scale(camera->front, camera->speed * deltatime));
		if(input_state.down)
			camera->position = glms_vec3_add(camera->position, glms_vec3_scale(camera->front, -camera->speed * deltatime));
		if(input_state.left)
			camera->position = glms_vec3_add(camera->position, glms_vec3_scale(glms_cross(camera->front, camera->up), -camera->speed * deltatime));
		if(input_state.right)
			camera->position = glms_vec3_add(camera->position, glms_vec3_scale(glms_cross(camera->front, camera->up), camera->speed * deltatime));
		if(input_state.space)
			camera->position = glms_vec3_add(camera->position, glms_vec3_scale(camera->up, camera->speed * deltatime));
		if(input_state.l_ctrl)
			camera->position = glms_vec3_add(camera->position, glms_vec3_scale(camera->up, -camera->speed * deltatime));
	}
	if(camera->camera_type & LOOK_AROUND)
	{
		if(game_engine.current_window.input_system.mouse_clicked_data[GLFW_MOUSE_BUTTON_LEFT])
		{
			glfwSetInputMode(game_engine.current_window.handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			vec2s mouse_position = game_engine.current_window.input_system.mouse_position;

			if(first_mouse)
			{
				last_x = mouse_position.x;
				last_y = mouse_position.y;
				first_mouse = false;
			}

			float xoffset = mouse_position.x - last_x;
			float yoffset = last_y - mouse_position.y;

			last_x = mouse_position.x;
			last_y = mouse_position.y;

			xoffset *= camera->mouse_sensitivity;
			yoffset *= camera->mouse_sensitivity;

			camera->yaw += xoffset;
			camera->pitch += yoffset;

			if(camera->pitch > 89.0f) camera->pitch = 89.0f;
			if(camera->pitch < -89.0f) camera->pitch = -89.0f;

			camera_direction.x = cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
			camera_direction.y = sin(glm_rad(camera->pitch));
			camera_direction.z = sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));

			camera->front = glms_vec3_normalize(camera_direction);
		}
		else
		{
			first_mouse = true;
			glfwSetInputMode(game_engine.current_window.handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}
