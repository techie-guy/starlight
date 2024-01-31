#include "ScenePlay.h"

#include "Components.h"
#include "Scene.h"
#include "Camera.h"
#include "Map.h"
#include "ECS.h"
#include "VertexAttributes.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "SpriteSheet.h"
#include "FontRenderer.h"

#if defined(_PLATFORM_DESKTOP)
	#include <glad/gles2.h>
#elif defined(_PLATFORM_WEB)
	#include <emscripten.h>
	#include <GLES3/gl3.h>
#elif defined(_PLATFORM_ANDROID)
	#include <GLES3/gl3.h>
#endif

#include <cglm/struct.h>
#include "cimgui.h"

static Window* current_window;

static InputState input_state;

static Camera camera;
static mat4s view_times_proj = GLMS_MAT4_IDENTITY_INIT;

static SpriteSheet* player_spritesheet;
static char* player_current_sprite_name = "";

static Quad player_quad[1] =
{
	(Quad)
	{
		{
		{{{1.0f, 1.0f, 0.0f}}, {{0.0f, 0.0f, 0.0f, 0.0f}}, {{1.0f, 1.0f}}},
		{{{1.0f, -1.0f, 0.0f}}, {{0.0f, 0.0f, 0.0f, 0.0f}}, {{1.0f, 0.0f}}},
		{{{-1.0f, -1.0f, 0.0f}}, {{0.0f, 0.0f, 0.0f, 0.0f}}, {{0.0f, 0.0f}}},
		{{{-1.0f, 1.0f, 0.0f}}, {{0.0f, 0.0f, 0.0f, 0.0f}}, {{0.0f, 1.0f}}},
		}
	}
};

static GLushort player_indices[6] =
{
	0, 1, 2,
	2, 3, 0
};

static Entity* player;

static void init_player()
{
	player = addEntity("player");

	player->component_list.transform_component = (Component_Transform)
	{
		.position = {5.0f, 5.0f, 0.0f},
		.scale = {1.0f, 1.0f, 0.0f},
		.speed = 4.0f,
	};

	player_spritesheet = (SpriteSheet*)hashmap_get(spriteSheetHashMap, &(SpriteSheet){ .name="villan" });

	player_current_sprite_name = player_spritesheet->defaultSprite;	

	initTextureFromFile(&player->component_list.sprite_component.texture, player_spritesheet->path);
	initVertexAttributes(&player->component_list.sprite_component.vertex_attribs, player_quad, sizeof(player_quad), player_indices, sizeof(player_indices));
	initShaderProgram(&player->component_list.sprite_component.shader_program, "assets/shaders/player-vertex-shader.glsl", "assets/shaders/player-fragment-shader.glsl");
}

static void player_update_texcoords()
{
	Sprite* currentSprite = (Sprite*)hashmap_get(player_spritesheet->spriteHashMap, &(Sprite){ .name=player_current_sprite_name });

	static int currentFrame = 0;

	currentFrame++;

	int animFrame;
	if(player_spritesheet->type == SHEET_HORIZONTAL)
		animFrame = currentSprite->x + ((currentFrame/currentSprite->frameSpeed) % currentSprite->spriteCount);
	if(player_spritesheet->type == SHEET_VERTICAL)
		animFrame = currentSprite->y + ((currentFrame/currentSprite->frameSpeed) % currentSprite->spriteCount);

	getSpriteAnimation(player_quad, player_spritesheet, currentSprite, animFrame);
}

static void draw_player()
{
	bindShaderProgram(&player->component_list.sprite_component.shader_program);

	bindTexture(&player->component_list.sprite_component.texture);

	mat4s transform = GLMS_MAT4_IDENTITY_INIT;
	transform = glms_scale(transform, player->component_list.transform_component.scale);
	transform = glms_translate(transform, player->component_list.transform_component.position);

//	mat4s mvp = glms_mat4_mulN((mat4s*[]){(mat4s*)&view_times_proj, &transform}, 2);

	uniformMat4(&player->component_list.sprite_component.shader_program, "projection", camera.projection_matrix);
	uniformMat4(&player->component_list.sprite_component.shader_program, "view", camera.view_matrix);
	uniformMat4(&player->component_list.sprite_component.shader_program, "transform", transform);

	bindBuffer(&player->component_list.sprite_component.vertex_attribs, VAO);
	bindBuffer(&player->component_list.sprite_component.vertex_attribs, VBO);

	player_update_texcoords();

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(player_quad), player_quad);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
}

static void init(Window* window)
{
	current_window = window;

	init_player();
	initMap();

	camera.window_dimensions = (vec2s){{ current_window->width, current_window->height }};
	camera.position = (vec3s){{5.0f, 5.0f, -10.0f}};
	camera.target = (vec3s){{0.0f, 0.0f, 0.0f}};
	camera.up = (vec3s){{0.0f, 1.0f, 0.0f}};
	camera.speed = 5.0f;
	initCamera(&camera);
}

static void update(float deltatime)
{
	camera.window_dimensions = (vec2s){{ current_window->width, current_window->height }};
	updateCamera(&camera);
	view_times_proj = glms_mat4_mulN((mat4s*[]){&camera.projection_matrix, &camera.view_matrix}, 2);

	if(!input_state.keyPress)
	{
		player_current_sprite_name = player_spritesheet->defaultSprite;
	}

	if(input_state.up)
	{
		player->component_list.transform_component.position.y += player->component_list.transform_component.speed * deltatime;
		player_current_sprite_name = "move-up";
	}
	if(input_state.down)
	{
		player->component_list.transform_component.position.y-= player->component_list.transform_component.speed * deltatime;
		player_current_sprite_name = "move-down";
	}
	if(input_state.left)
	{
		player->component_list.transform_component.position.x += player->component_list.transform_component.speed * deltatime;
		player_current_sprite_name = "move-left";
	}
	if(input_state.right)
	{
		player->component_list.transform_component.position.x -= player->component_list.transform_component.speed * deltatime;
		player_current_sprite_name = "move-right";
	}

	moveCamera(&camera, input_state, deltatime);
}

static void render()
{
//	ImGui_ShowDemoWindow(NULL);	

	drawMap(&camera);
	draw_player();

	renderText("Hello", 100.0f, 300.0f, 1.0f, (vec3s){1.0f, 1.0f, 1.0f});
}

static void process_input(InputSystem input_system, float deltatime)
{
	input_state.up = input_system.key_pressed_data[GLFW_KEY_UP] || input_system.key_pressed_data[GLFW_KEY_W];
	input_state.down = input_system.key_pressed_data[GLFW_KEY_DOWN] || input_system.key_pressed_data[GLFW_KEY_S];
	input_state.left = input_system.key_pressed_data[GLFW_KEY_LEFT] || input_system.key_pressed_data[GLFW_KEY_A];
	input_state.right = input_system.key_pressed_data[GLFW_KEY_RIGHT] || input_system.key_pressed_data[GLFW_KEY_D];
	input_state.space = input_system.key_pressed_data[GLFW_KEY_SPACE];
	input_state.l_ctrl = input_system.key_pressed_data[GLFW_KEY_LEFT_CONTROL];

	// Joystick
	ImVec2 joystick_button_size = {40.0f, 40.0f};

    ImGui_SetNextWindowPos((ImVec2){50, ImGui_GetIO()->DisplaySize.y - 200}, ImGuiCond_Always);

	ImGui_Begin("Input", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	{
		ImGui_BeginGroup();
		ImGui_Text("    ");
		ImGui_SameLine();

		if(ImGui_ButtonEx("", joystick_button_size)) input_state.up = true;
		else if(ImGui_IsItemActive()) input_state.up = true;

		if(ImGui_ButtonEx("", joystick_button_size)) input_state.left = true;
		else if(ImGui_IsItemActive()) input_state.left = true;

		ImGui_SameLine();
		if(ImGui_ButtonEx("", joystick_button_size)) input_state.l_ctrl = true;
		else if(ImGui_IsItemActive()) input_state.l_ctrl = true;

		ImGui_SameLine();
		if(ImGui_ButtonEx("", joystick_button_size)) input_state.right = true;
		else if(ImGui_IsItemActive()) input_state.right = true;

		ImGui_Text("    ");
		ImGui_SameLine();
		if(ImGui_ButtonEx("", joystick_button_size)) input_state.down = true;
		else if(ImGui_IsItemActive()) input_state.down = true;

		ImGui_EndGroup();
	}
	ImGui_End();
}

static void activate()
{
}

static void deactivate()
{
}

static void destroy()
{
	destroyMap();
	
	destroyTexture(&player->component_list.sprite_component.texture);
	destroyVertexAttributes(&player->component_list.sprite_component.vertex_attribs);
	destroyShaderProgram(&player->component_list.sprite_component.shader_program);
}

Scene ScenePlay = {"ScenePlay", init, destroy, activate, deactivate, update, render, process_input};
