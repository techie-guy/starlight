#include "SceneModel.h"
#include "Application.h"
#include "Scene.h"
#include "ECS.h"
#include "VertexAttributes.h"
#include "ShaderProgram.h"

#include "Camera.h"

#include "3D-Loader.h"

#include <stb_ds.h>

static Model model;


static Entity* model_scene;
static Vertex* vertex_render_data = NULL;
static GLushort* index_data = NULL;

static Camera camera;

static InputState input_state;


static void init()
{
	model_scene = add_entity("Model");

	model.data = NULL;

	model.type = MODEL_GLTF;
	model_load_from_file(&model, "3d-models/skull.glb");
	model_parse_data(&model, &vertex_render_data, &index_data);
	model_load_texture(&model, &model_scene->component_list.sprite_component.texture);
	

	init_vertex_attributes(&model_scene->component_list.sprite_component.vertex_attribs, vertex_render_data, sizeof(Vertex)*model.vertex_count, index_data, sizeof(GLushort)*model.index_count, true);
	init_shader_program(&model_scene->component_list.sprite_component.shader_program, "shaders/model-vertex-shader.glsl", "shaders/model-fragment-shader.glsl");

	camera.position = (vec3s){{-10.0f, 0.0f, 20.0f}};
	camera.target = (vec3s){{0.0f, 0.0f, 0.0f}};
	camera.up = (vec3s){{0.0f, 1.0f, 0.0f}};
	camera.front = (vec3s){{0.0f, 0.0f, -1.0f}};
	camera.speed = 5.0f;
	camera.fov = glm_rad(45.0f);
	camera.near_plane = 0.1f;
	camera.far_plane = 1000.0f;
	camera.mouse_sensitivity = 0.1f;
	camera.pitch = 0.0f;
	camera.yaw = -90.0f;
	camera.camera_type =  WALK_AROUND | LOOK_AROUND;
	init_camera(&camera);
}

static void render()
{	
	mat4s transform = GLMS_MAT4_IDENTITY_INIT;
//	transform = glms_scale(transform, (vec3s){10.0f, 10.0f, 10.0f});
	static float angle = 0.0f;
	transform = glms_rotate(transform, glm_rad(angle), (vec3s){0.0f, 1.0f, 0.0f});
	angle -= 1.0f;


//	cgltf_node_transform_local(&model.data->nodes[0], (float*)transform.raw);

	bind_texture(&model_scene->component_list.sprite_component.texture);

	bind_shader_program(&model_scene->component_list.sprite_component.shader_program);
	uniform_mat4(&model_scene->component_list.sprite_component.shader_program, "projection", camera.projection_matrix);
	uniform_mat4(&model_scene->component_list.sprite_component.shader_program, "view", camera.view_matrix);
	uniform_mat4(&model_scene->component_list.sprite_component.shader_program, "transform", transform);


	bind_vertex_buffer(&model_scene->component_list.sprite_component.vertex_attribs, VAO);
//	glDrawArrays(GL_TRIANGLES, 0, arrlen(vertex_render_data));
	glDrawElements(GL_TRIANGLES, model.index_count, GL_UNSIGNED_SHORT, 0);
}

static void update()
{
	update_camera(&camera);
}

static void process_input()
{
	InputSystem input_system = game_engine.current_window.input_system;

	input_state.up = input_system.key_pressed_data[GLFW_KEY_UP] || input_system.key_pressed_data[GLFW_KEY_W];
	input_state.down = input_system.key_pressed_data[GLFW_KEY_DOWN] || input_system.key_pressed_data[GLFW_KEY_S];
	input_state.left = input_system.key_pressed_data[GLFW_KEY_LEFT] || input_system.key_pressed_data[GLFW_KEY_A];
	input_state.right = input_system.key_pressed_data[GLFW_KEY_RIGHT] || input_system.key_pressed_data[GLFW_KEY_D];
	input_state.space = input_system.key_pressed_data[GLFW_KEY_SPACE];
	input_state.l_ctrl = input_system.key_pressed_data[GLFW_KEY_LEFT_CONTROL];

	// Joystick
	/*
	if(is_joystick_active)
	{
		if(joystick_angle >= 315.0f || joystick_angle <= 45.0f) input_state.right = true;
		else if(joystick_angle >= 45.0f && joystick_angle <= 135.0f) input_state.up = true;
		else if(joystick_angle >= 135.0f && joystick_angle <= 225.0f) input_state.left = true;
		else if(joystick_angle >= 225.0f && joystick_angle <= 315.0f) input_state.down = true;
	}*/

	move_camera(&camera, input_state, game_engine.deltatime);

}

static void activate()
{
}

static void deactivate()
{
}

static void destroy()
{
//	free(index_data);

	model_free(&model);
	destroy_texture(&model_scene->component_list.sprite_component.texture);
	destroy_shader_program(&model_scene->component_list.sprite_component.shader_program);
	destroy_vertex_attributes(&model_scene->component_list.sprite_component.vertex_attribs, true);
}


Scene SceneModel = {"SceneModel", init, destroy, activate, deactivate, update, render, process_input};
