#include "ScenePlayServer.h"
#include "Components.h"
#include "Network.h"
#include "Scene.h"
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

static Server server;
static ENetPeer* peer;
static ENetEvent event;
static struct { int key; Player_Packet value; }* players = NULL;
static int player_packet_length = 0;
static unsigned char player_data_buffer[2048];

static bool is_server_running = false;

static InputState input_state;

static Camera camera;

static float joystick_angle = 0.0f;
static bool is_joystick_active = false;
static ImVec2 joystick_box_size = {400.0f, 400.0f};
static float joystick_radius = 70.0f;
static ImVec4 joystick_color = {225.0f, 225.0f, 225.0f, 100.0f};

static ImGuiIO* imgui_io;

// Map
#define MAP_SIZE_X 50
#define MAP_SIZE_Y 50
#define MAP_TILE_COUNT MAP_SIZE_X * MAP_SIZE_Y

static Entity* map;
static SpriteSheet* map_sprite_sheet;
static Quad map_tiles[MAP_TILE_COUNT];
static GLushort map_indices[MAP_TILE_COUNT * 6];

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

	init_map();

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

	
	imgui_io = ImGui_GetIO();
}

static void update()
{
	update_camera(&camera);

	move_camera(&camera, input_state, game_engine.deltatime);

	for(int i = 0; i < hmlen(players); i++)
	{
		unsigned char buf[152];
		int packet_size = pack(buf, "shm#", players[i].value.name, players[i].value.id, players[i].value.transform, players[i].value.vertex_data, 6);
		memcpy(player_data_buffer + i * packet_size, buf, packet_size);
	}


	if(hmlen(players) > 0)
	{
		ENetPacket* packet = enet_packet_create(player_data_buffer, 152*hmlen(players), ENET_PACKET_FLAG_RELIABLE);
		broadcast_packet(server.host, 0, packet);
	}
}

static void render()
{
#if defined(_PLATFORM_ANDROID) || defined(_PLATFORM_WEB)
	ui_component_joystick("Input", "Joystick", joystick_box_size, joystick_radius, joystick_color, &joystick_angle, &is_joystick_active);
#endif	

	while(is_server_running && enet_host_service(server.host, &event, 0) > 0)
	{
		switch(event.type)
		{
			case ENET_EVENT_TYPE_CONNECT:
			{
				log_debug("Client Connected: %u\n", event.peer->address.port);

				Player_Packet player_packet;

				player_packet.id = hmlen(players);

				send_int_packet(server.host, event.peer, 0, player_packet.id);

				hmput(players, player_packet.id, player_packet);
				
				event.peer->data = (int)player_packet.id;
	
				break;
			}
			case ENET_EVENT_TYPE_RECEIVE:
			{
				int player_id;
				mat4s player_transform;
				char player_name[20];
				Vertex vertex_data[6];

				unpack(event.packet->data, "shm#", player_name, &player_id, &player_transform, vertex_data, 6);

				strcpy(players[player_id].value.name, player_name);
				players[player_id].value.transform = player_transform;
				memcpy(players[player_id].value.vertex_data, vertex_data, 6*sizeof(Vertex));
	
				enet_packet_destroy(event.packet);
				break;
			}
			case ENET_EVENT_TYPE_DISCONNECT:
			{
				hmdel(players, (int)event.peer->data);
				log_debug("Player %d disconnected!\n", (int)event.peer->data);

				event.peer->data = NULL;
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

		if(ImGui_Button("Start Server"))
		{
			if(!is_server_running)
			{
				init_network_server(&server, 1234, 32, 2);
				is_server_running = true;
			}
		}

		if(ImGui_Button("Stop Server"))
		{
			if(is_server_running)
			{
				destroy_network_server(&server);
				is_server_running = false;

				players = NULL;
				memset(player_data_buffer, 0, 2048);
			}
		}
	}
	ImGui_End();


	draw_map();
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
	if(is_server_running)
	{
		destroy_network_server(&server);
		is_server_running = false;
	}

	destroy_texture(&map->component_list.sprite_component.texture);
	destroy_shader_program(&map->component_list.sprite_component.shader_program);
	destroy_vertex_attributes(&map->component_list.sprite_component.vertex_attribs, true);
}

Scene ScenePlayServer = {"ScenePlayServer", init, destroy, activate, deactivate, update, render, process_input};
