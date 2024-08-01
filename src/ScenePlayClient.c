#include "ScenePlayClient.h"

#include "Components.h"
#include "Scene.h"
#include "Network.h"
#include "Camera.h"
#include "ECS.h"
#include "UI-Imgui.h"
#include "Utils.h"
#include "VertexAttributes.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "SpriteSheet.h"
#include "FontRenderer.h"
#include "Application.h"
#include "NetPacker.h"

#if defined(_PLATFORM_DESKTOP)
	#include <glad/gles2.h>
#elif defined(_PLATFORM_WEB)
	#include <emscripten.h>
	#include <GLES3/gl3.h>
#elif defined(_PLATFORM_ANDROID)
	#include <GLES3/gl3.h>
#endif

#include <math.h>
#include <time.h>

#include <hashmap.h>
#include <stb_perlin.h>
#include <stb_ds.h>

#include <enet.h>

#include <cglm/struct.h>
#include "cimgui.h"

static Window* current_window;

static Client client;
static ENetEvent event;
static ENetPeer* peer = NULL;
static unsigned char buffer[1024];
static Player_Packet player_packet;
static unsigned int player_packet_length = 0;
static struct { int key; Player_Packet value; }* players = NULL;

static Vertex* server_render_data = NULL;

static bool is_server_joined = false;

static InputState input_state;

static Camera camera;

static float joystick_angle = 0.0f;
static bool is_joystick_active = false;
static ImVec2 joystick_box_size = {400.0f, 400.0f};
static float joystick_radius = 70.0f;
static ImVec4 joystick_color = {225.0f, 225.0f, 225.0f, 100.0f};

static ImGuiIO* imgui_io;

static Entity* network_render;

// Player
static SpriteSheet* player_spritesheet;
static char* player_current_sprite_name = "";
static Entity* player;

static Vertex player_vertex_data[6] =
{
	{{{1.0f, 1.0f, 0.0f}}, {{0.0f, 0.0f, 0.0f, 0.0f}}, {{1.0f, 1.0f}}},
	{{{1.0f, -1.0f, 0.0f}}, {{0.0f, 0.0f, 0.0f, 0.0f}}, {{1.0f, 0.0f}}},
	{{{-1.0f, -1.0f, 0.0f}}, {{0.0f, 0.0f, 0.0f, 0.0f}}, {{0.0f, 0.0f}}},
	
	{{{-1.0f, -1.0f, 0.0f}}, {{0.0f, 0.0f, 0.0f, 0.0f}}, {{0.0f, 0.0f}}},
	{{{-1.0f, 1.0f, 0.0f}}, {{0.0f, 0.0f, 0.0f, 0.0f}}, {{0.0f, 1.0f}}},
	{{{1.0f, 1.0f, 0.0f}}, {{0.0f, 0.0f, 0.0f, 0.0f}}, {{1.0f, 1.0f}}},
};

// Map
#define MAP_SIZE_X 50
#define MAP_SIZE_Y 50
#define MAP_TILE_COUNT MAP_SIZE_X * MAP_SIZE_Y

static Entity* map;
static SpriteSheet* map_sprite_sheet;
static Quad map_tiles[MAP_TILE_COUNT];
static GLushort map_indices[MAP_TILE_COUNT * 6];

static void update_texcoords(Vertex vertex_data[], int start)
{
	Sprite* current_sprite = (Sprite*)hashmap_get(player_spritesheet->sprite_hashmap, &(Sprite){ .name=player_current_sprite_name });

	static int current_frame = 0;

	current_frame++;

	int anim_frame;
	if(player_spritesheet->type == SHEET_HORIZONTAL)
		anim_frame = current_sprite->x + ((current_frame/current_sprite->frame_speed) % current_sprite->sprite_count);
	if(player_spritesheet->type == SHEET_VERTICAL)
		anim_frame = current_sprite->y + ((current_frame/current_sprite->frame_speed) % current_sprite->sprite_count);

	get_sprite_animation_vert(vertex_data, start, player_spritesheet, current_sprite, anim_frame);
}


// Network

static void init_network_render()
{
	network_render = add_entity("network");
	init_texture_from_file(&network_render->component_list.sprite_component.texture, player_spritesheet->path);

	init_vertex_attributes(&network_render->component_list.sprite_component.vertex_attribs, server_render_data, sizeof(Vertex)*6*5, NULL, 0, false);
	init_shader_program(&network_render->component_list.sprite_component.shader_program, "shaders/player-vertex-shader.glsl", "shaders/player-fragment-shader.glsl");
}

static void draw_network_render()
{
	bind_shader_program(&network_render->component_list.sprite_component.shader_program);

	uniform_mat4(&network_render->component_list.sprite_component.shader_program, "projection", camera.projection_matrix);
	uniform_mat4(&network_render->component_list.sprite_component.shader_program, "view", camera.view_matrix);


	bind_texture(&network_render->component_list.sprite_component.texture);

	bind_vertex_buffer(&network_render->component_list.sprite_component.vertex_attribs, VAO);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex)*arrlen(server_render_data), server_render_data);
	
	for(int i = 0; i < arrlen(server_render_data); i+=6)
	{
		uniform_mat4(&network_render->component_list.sprite_component.shader_program, "transform", players[i].value.transform);
		glDrawArrays(GL_TRIANGLES, i, 6);
	}
}

// Player
static void init_player()
{
	player = add_entity("player");

	player->component_list.transform_component = (Component_Transform)
	{
		.position = {0.0f, 0.0f, 0.1f},
		.scale = {1.0f, 1.0f, 1.0f},
		.speed = 4.0f,
	};
	
	player_spritesheet = (SpriteSheet*)hashmap_get(sprite_sheet_hashmap, &(SpriteSheet){ .name="villan" });

	player_current_sprite_name = player_spritesheet->default_sprite;

	init_texture_from_file(&player->component_list.sprite_component.texture, player_spritesheet->path);
	init_vertex_attributes(&player->component_list.sprite_component.vertex_attribs, player_vertex_data, sizeof(Vertex)*6, NULL, 0, false);
	init_shader_program(&player->component_list.sprite_component.shader_program, "shaders/player-vertex-shader.glsl", "shaders/player-fragment-shader.glsl");
}

static void draw_player()
{
	bind_shader_program(&player->component_list.sprite_component.shader_program);
	
	player->component_list.transform_component.transform = (mat4s)GLMS_MAT4_IDENTITY_INIT;

	player->component_list.transform_component.transform = glms_scale(player->component_list.transform_component.transform, player->component_list.transform_component.scale);
	player->component_list.transform_component.transform = glms_translate(player->component_list.transform_component.transform, player->component_list.transform_component.position);

	update_texcoords(player_vertex_data, 0);

	uniform_mat4(&player->component_list.sprite_component.shader_program, "projection", camera.projection_matrix);
	uniform_mat4(&player->component_list.sprite_component.shader_program, "view", camera.view_matrix);
	uniform_mat4(&player->component_list.sprite_component.shader_program, "transform", player->component_list.transform_component.transform);


	bind_texture(&player->component_list.sprite_component.texture);
	
	bind_vertex_buffer(&player->component_list.sprite_component.vertex_attribs, VAO);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex)*6, player_vertex_data);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

// Map
static void fill_map()
{
	srand(time(NULL));
	const int seed = rand();

	// Vertices
	for(int y = 0; y < MAP_SIZE_Y; y++)
	{
		for(int x = 0; x < MAP_SIZE_X; x++)
		{
			float noise_x = (float)x/(float)MAP_SIZE_X;
			float noise_y = (float)y/(float)MAP_SIZE_Y;
			float noise_factor = 10.0f;
			float scale = 0.1;
			static char* tile_sprite = "grass";

			float perlin_noise = stb_perlin_noise3_seed(noise_x * noise_factor, noise_y * noise_factor, 0.0f, 0, 0, 0, seed);

			if(perlin_noise < -0.2) tile_sprite = "water";
			if(perlin_noise > -0.1) tile_sprite = "grass";
			if(perlin_noise > 0.3) tile_sprite = "stone";

			Sprite* current_tile_sprite = (Sprite*)hashmap_get(map_sprite_sheet->sprite_hashmap, &(Sprite){ .name=tile_sprite });

			float x1 = (float)(x * current_tile_sprite->sprite_width * scale);
			float y1 = (float)(y * current_tile_sprite->sprite_height * scale);
			float x2 = (float)((x+1) * current_tile_sprite->sprite_width * scale);
			float y2 = (float)((y+1) * current_tile_sprite->sprite_height * scale);

			UV_Coords uv_coords;
			Sprite* sprite = (Sprite*)hashmap_get(map_sprite_sheet->sprite_hashmap, &(Sprite){ .name=tile_sprite });
			get_spriteUV(&uv_coords, map_sprite_sheet, sprite);

			map_tiles[y * MAP_SIZE_X + x] = (Quad)
			{
				{
					{{{x2, y2, 0.0f}}, {{1.0f, 0.0f, 0.0f, 1.0f}}, {{uv_coords.x2, uv_coords.y2}}},
					{{{x2, y1, 0.0f}}, {{1.0f, 0.0f, 0.0f, 1.0f}}, {{uv_coords.x2, uv_coords.y1}}},
					{{{x1, y1, 0.0f}}, {{1.0f, 0.0f, 0.0f, 1.0f}}, {{uv_coords.x1, uv_coords.y1}}},
					{{{x1, y2, 0.0f}}, {{1.0f, 0.0f, 0.0f, 1.0f}}, {{uv_coords.x1, uv_coords.y2}}},
				}
			};
		}
	}

	// Indices
	for(int i = 0; i < sizeof(map_indices)/sizeof(map_indices[0]); i += 6)
	{
		static int offset = 0;

		map_indices[i + 0] = 0 + offset;
		map_indices[i + 1] = 1 + offset;
		map_indices[i + 2] = 2 + offset;

		map_indices[i + 3] = 2 + offset;
		map_indices[i + 4] = 3 + offset;
		map_indices[i + 5] = 0 + offset;

		offset += 4;
	}
}

static void init_map()
{
	map = add_entity("map");
	map->component_list.transform_component = (Component_Transform)
	{
		.position = {0.0f, 0.0f, 0.0f},
		.scale = {1.0f, 1.0f, 0.0f},
		.speed = 0.0f,
	};

	map_sprite_sheet = (SpriteSheet*)hashmap_get(sprite_sheet_hashmap, &(SpriteSheet){ .name="tiles" });

	init_texture_from_file(&map->component_list.sprite_component.texture, map_sprite_sheet->path);

	fill_map();
	init_shader_program(&map->component_list.sprite_component.shader_program, "shaders/map-vertex-shader.glsl", "shaders/map-fragment-shader.glsl");
	init_vertex_attributes(&map->component_list.sprite_component.vertex_attribs, map_tiles, sizeof(map_tiles), map_indices, sizeof(map_indices), true);
}

static void draw_map()
{
	bind_shader_program(&map->component_list.sprite_component.shader_program);

	bind_texture(&map->component_list.sprite_component.texture);

	mat4s transform = GLMS_MAT4_IDENTITY_INIT;
	transform = glms_scale(transform, (vec3s){{1.0f, 1.0f, 0.0f}});
	transform = glms_translate(transform, (vec3s){{0.0f, 0.0f, 0.0f}});

	uniform_mat4(&map->component_list.sprite_component.shader_program, "projection", camera.projection_matrix);
	uniform_mat4(&map->component_list.sprite_component.shader_program, "view", camera.view_matrix);
	uniform_mat4(&map->component_list.sprite_component.shader_program, "transform", transform);

	bind_vertex_buffer(&map->component_list.sprite_component.vertex_attribs, VAO);

	glDrawElements(GL_TRIANGLES, MAP_TILE_COUNT*6, GL_UNSIGNED_SHORT, 0);
}

// Scene
static void init()
{
	current_window = &game_engine.current_window;

	init_player();
	init_map();
	
	init_network_client(&client, 2);
	init_network_render();

	camera.position = (vec3s){{0.0f, 0.0f, 10.0f}};
	camera.target = (vec3s){{0.0f, 0.0f, 0.0f}};
	camera.up = (vec3s){{0.0f, 1.0f, 0.0f}};
	camera.speed = 5.0f;
	camera.front = (vec3s){{0.0f, 0.0f, -1.0f}};
	camera.fov = glm_rad(45.0f);
	camera.near_plane = 0.1f;
	camera.far_plane = 1000.0f;
	camera.mouse_sensitivity = 0.1f;
	camera.pitch = 0.0f;
	camera.yaw = -90.0f;
//	camera.camera_type = WALK_AROUND;
	init_camera(&camera);

	player_packet = (Player_Packet)
	{
		"Player 1",
		-1,
	};

	
	imgui_io = ImGui_GetIO();
}

static void update()
{
	update_camera(&camera);

	player_packet.transform = player->component_list.transform_component.transform;

	memcpy(player_packet.vertex_data, player_vertex_data, sizeof(Vertex)*6);
	
	player_packet_length = pack(buffer, "shm#", player_packet.name, player_packet.id, player_packet.transform, player_packet.vertex_data, 6);


	if(!input_state.keyPress)
	{
		player_current_sprite_name = player_spritesheet->default_sprite;
	}

	if(input_state.up)
	{
		player->component_list.transform_component.position.y += player->component_list.transform_component.speed * game_engine.deltatime;
		player_current_sprite_name = "move-up";
	}
	if(input_state.down)
	{
		player->component_list.transform_component.position.y -= player->component_list.transform_component.speed * game_engine.deltatime;
		player_current_sprite_name = "move-down";
	}
	if(input_state.left)
	{
		player->component_list.transform_component.position.x -= player->component_list.transform_component.speed * game_engine.deltatime;
		player_current_sprite_name = "move-left";
	}
	if(input_state.right)
	{
		player->component_list.transform_component.position.x += player->component_list.transform_component.speed * game_engine.deltatime;
		player_current_sprite_name = "move-right";
	}

	move_camera(&camera, input_state, game_engine.deltatime);

	camera.position.x = player->component_list.transform_component.position.x;
	camera.position.y = player->component_list.transform_component.position.y;
}

static void render()
{
#if defined(_PLATFORM_ANDROID) || defined(_PLATFORM_WEB)
	ui_component_joystick("Input", "Joystick", joystick_box_size, joystick_radius, joystick_color, &joystick_angle, &is_joystick_active);
#endif

	if(is_server_joined)
	{
		send_packet(client.host, peer, 0, buffer, player_packet_length);
	}


	while(is_server_joined && enet_host_service(client.host, &event, 0) > 0)
	{
		switch(event.type)
		{
			case ENET_EVENT_TYPE_CONNECT:
			{
				log_debug("Connected: %u\n", event.peer->address.port);	
				
				break;
			}
			case ENET_EVENT_TYPE_RECEIVE:
			{
				if(player_packet.id < 0) unpack(event.packet->data, "h", &player_packet.id);

				server_render_data = NULL;

				for(int i = 0; i < event.packet->dataLength/player_packet_length; i++)
				{
//					if(hmlen(players) != event.packet->dataLength/90)
					
					Player_Packet player_packet_buf = {};
					Vertex vertex_data[6];
					char player_name[20];

					unsigned char buf[player_packet_length];
					memcpy(buf, event.packet->data + i*player_packet_length, player_packet_length);


					unpack(buf, "shm#", player_name, &player_packet_buf.id, &player_packet_buf.transform, vertex_data, 6);
					memcpy(player_packet_buf.vertex_data, vertex_data, sizeof(Vertex)*6);
					strcpy(player_packet_buf.name, player_name);

					if(player_packet_buf.id == player_packet.id) continue;

					arrput(server_render_data, vertex_data[0]);
					arrput(server_render_data, vertex_data[1]);
					arrput(server_render_data, vertex_data[2]);
					arrput(server_render_data, vertex_data[3]);
					arrput(server_render_data, vertex_data[4]);
					arrput(server_render_data, vertex_data[5]);

					hmput(players, player_packet_buf.id, player_packet_buf);
				}

				enet_packet_destroy(event.packet);
				break;
			}
			case ENET_EVENT_TYPE_DISCONNECT:
			{
				send_int_packet(client.host, peer, player_packet.id, 0);
				players = NULL;
				server_render_data = NULL;

				log_debug("Disconnected\n");
			}
			case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
			{
				break;
			}
			case ENET_EVENT_TYPE_NONE:
			{
				break;
			}
		}
	}

	
	ImGui_Begin("Menu", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse);
	{
		ImGui_SetWindowSize((ImVec2){0.0f, 0.0f}, ImGuiCond_Always);
		ImGui_SetWindowPos((ImVec2){0.0f, 0.0f}, ImGuiCond_Always);

		if(ImGui_Button("Join Server"))
		{

				network_client_connect_peer(&client, "localhost", 1234, 0, &peer, &event);

				is_server_joined = true;
		}

		if(ImGui_Button("Leave Server"))
		{
			disconnect_peer(peer);
			is_server_joined = false;
		}
	}
	ImGui_End();


	draw_map();
	draw_player();
	draw_network_render();

	if(is_server_joined)
	{
		char text[2];
		sprintf(text, "%d", player_packet.id);

		render_text(text, 100.0f, 300.0f, 1.0f, "#ffffff", 1.0f);
	}
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
	if(is_joystick_active)
	{
		if(joystick_angle >= 315.0f || joystick_angle <= 45.0f) input_state.right = true;
		else if(joystick_angle >= 45.0f && joystick_angle <= 135.0f) input_state.up = true;
		else if(joystick_angle >= 135.0f && joystick_angle <= 225.0f) input_state.left = true;
		else if(joystick_angle >= 225.0f && joystick_angle <= 315.0f) input_state.down = true;
	}
}

static void activate()
{
}

static void deactivate()
{
}

static void destroy()
{
	if(is_server_joined)
	{
		destroy_network_client(&client);
		is_server_joined = false;
	}

	destroy_texture(&network_render->component_list.sprite_component.texture);
	destroy_shader_program(&network_render->component_list.sprite_component.shader_program);
	destroy_vertex_attributes(&network_render->component_list.sprite_component.vertex_attribs, true);

	destroy_texture(&map->component_list.sprite_component.texture);
	destroy_shader_program(&map->component_list.sprite_component.shader_program);
	destroy_vertex_attributes(&map->component_list.sprite_component.vertex_attribs, true);
	
	destroy_texture(&player->component_list.sprite_component.texture);
	destroy_vertex_attributes(&player->component_list.sprite_component.vertex_attribs, true);
	destroy_shader_program(&player->component_list.sprite_component.shader_program);
}

Scene ScenePlayClient = {"ScenePlayClient", init, destroy, activate, deactivate, update, render, process_input};
