#include "SceneBlock.h"

#include "Application.h"
#include "ECS.h"
#include "FontRenderer.h"
#include "Utils.h"
#include "VertexAttributes.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Camera.h"
#include "UI-Imgui.h"
#include "SpriteSheet.h"

#include <string.h>
#include <time.h>

#include <cglm/struct.h>
#include <stb_ds.h>
#include <stb_perlin.h>


typedef struct
{
	vec3s position;
	Vertex* vertex_data;
} Block;

static Block* blocks;


static float joystick_angle = 0.0f;
static bool is_joystick_active = false;
static ImVec2 joystick_box_size = {400.0f, 400.0f};
static float joystick_radius = 70.0f;
static ImVec4 joystick_color = {225.0f, 225.0f, 225.0f, 100.0f};

static Window* current_window;
static Camera camera;

static Vertex* vertex_render_data;
//static vec3s* block_positions;

static SpriteSheet* block_sprite_sheet;

static Entity* block;
static InputState input_state;

//static void add_block(vec3s origin, float block_size, char* sprite_name, Vertex** vertex_data)
static void add_block(vec3s origin, float block_size, char* sprite_name)
{
	Block block = {};
	block.position = origin;

	vec3s cube_vertices_positions[8] =
	{
		(vec3s){-block_size, block_size, block_size},
		(vec3s){block_size, block_size, block_size},
		(vec3s){-block_size, -block_size, block_size},
		(vec3s){block_size, -block_size, block_size},
		(vec3s){-block_size, block_size, -block_size},
		(vec3s){block_size, block_size, -block_size},
		(vec3s){-block_size, -block_size, -block_size},
		(vec3s){block_size, -block_size, -block_size},
	};

	GLushort cube_indices[36] =
	{
		1, 0, 2, 3, 1, 2,    // Back face
		5, 1, 3, 7, 5, 3,    // Right face
		7, 6, 4, 5, 7, 4,    // Front face
		0, 4, 6, 2, 0, 6,    // Left face
		5, 4, 0, 1, 5, 0,    // Top face
		3, 2, 6, 7, 3, 6     // Bottom face
	};

	Sprite* sprite = (Sprite*)hashmap_get(block_sprite_sheet->sprite_hashmap, &(Sprite){ .name=sprite_name });
	UV_Coords uv_coords;

	get_spriteUV(&uv_coords, block_sprite_sheet, sprite);

	vec2s tex_coords[6];
	tex_coords[0] = (vec2s){uv_coords.x2, uv_coords.y2};
	tex_coords[1] = (vec2s){uv_coords.x1, uv_coords.y2};
	tex_coords[2] = (vec2s){uv_coords.x1, uv_coords.y1};

	tex_coords[4] = (vec2s){uv_coords.x2, uv_coords.y2};
	tex_coords[3] = (vec2s){uv_coords.x2, uv_coords.y1};
	tex_coords[5] = (vec2s){uv_coords.x1, uv_coords.y1};

	for(int i = 0; i < sizeof(cube_indices)/sizeof(cube_indices[0]); i++)
	{
		Vertex vertex;
		vertex.position = glms_vec3_add(cube_vertices_positions[cube_indices[i]], origin);
		vertex.tex_coord = tex_coords[i % 6];

//		arrput(*vertex_data, vertex);
		arrput(block.vertex_data, vertex);
	}

	arrput(blocks, block);
}

static void cull_faces(Block* block, float offset)
{
	// Linear Seach ;)
	for(int i = 0; i < arrlen(blocks); i++)
	{
		if(glms_vec3_eqv((vec3s){block->position.x-offset, block->position.y, block->position.z}, blocks[i].position)) // left
		{
			for(int j = 0; j < 6; j++)
			{
				block->vertex_data[18 + j] = (Vertex){};
			}
		}
		if(glms_vec3_eqv((vec3s){block->position.x+offset, block->position.y, block->position.z}, blocks[i].position)) // right
		{
			for(int j = 0; j < 6; j++)
			{
				block->vertex_data[6 + j] = (Vertex){};
			}
		}
		if(glms_vec3_eqv((vec3s){block->position.x, block->position.y+offset, block->position.z}, blocks[i].position)) // Top
		{
			for(int j = 0; j < 6; j++)
			{
				block->vertex_data[24 + j] = (Vertex){};
			}
		}
		if(glms_vec3_eqv((vec3s){block->position.x, block->position.y-offset, block->position.z}, blocks[i].position)) // Bottom
		{
			for(int j = 0; j < 6; j++)
			{
				block->vertex_data[30 + j] = (Vertex){};
			}
		}
		if(glms_vec3_eqv((vec3s){block->position.x, block->position.y, block->position.z-offset}, blocks[i].position)) // Front
		{
			for(int j = 0; j < 6; j++)
			{
				block->vertex_data[12 + j] = (Vertex){};
			}
		}
		if(glms_vec3_eqv((vec3s){block->position.x, block->position.y, block->position.z+offset}, blocks[i].position)) // Back
		{
			for(int j = 0; j < 6; j++)
			{
				block->vertex_data[0 + j] = (Vertex){};
			}
		}
	}
}

static void generate_map()
{
	int BLOCKS_X = 10;
	int BLOCKS_Y = 10;
	int BLOCKS_Z = 10;
	float HALF_BLOCK_SIZE = 1.0f;

	srand(time(NULL));
	const int seed = rand();

	for(int z = 0; z < BLOCKS_Z; z++)
	{
		for(int y = 0; y < BLOCKS_Y; y++)
		{
			for(int x = 0; x < BLOCKS_X; x++)
			{
				vec3s current_block_origin = glms_vec3_scale((vec3s){x, y, z}, 2*HALF_BLOCK_SIZE);

				float noise_x = (float)x/(float)BLOCKS_X;
				float noise_y = (float)y/(float)BLOCKS_Y;
				float noise_z = (float)z/(float)BLOCKS_Z;
				float noise_factor = 1.0f;
				static char* block_sprite = "grass";

				float perlin_noise = stb_perlin_noise3_seed(noise_x * noise_factor, noise_y * noise_factor, noise_z * noise_factor, 0, 0, 0, seed);
//				log_debug("Perlin Noise: %f\n", perlin_noise);

				if(perlin_noise > 0.2f) block_sprite = "grass";
				if(perlin_noise >= 0.0f && perlin_noise <= 0.2f) block_sprite = "water";
				if(perlin_noise < 0.0f) block_sprite = "stone";

				add_block(current_block_origin, HALF_BLOCK_SIZE, block_sprite);
			}
		}
	}

	for(int i = 0; i < arrlen(blocks); i++)
	{
		cull_faces(&blocks[i], 2 * HALF_BLOCK_SIZE);
	}
	
	for(int i = 0; i < arrlen(blocks); i++)
	{
		for(int j = 0; j < arrlen(blocks[i].vertex_data); j++)
		{
			arrput(vertex_render_data, blocks[i].vertex_data[j]);
		}
	}
}

static void init()
{
	current_window = &game_engine.current_window;

	block = add_entity("Block");

	block_sprite_sheet = (SpriteSheet*)hashmap_get(sprite_sheet_hashmap, &(SpriteSheet){ .name="tiles" });

	generate_map();

	init_texture_from_file(&block->component_list.sprite_component.texture, block_sprite_sheet->path);
	init_vertex_attributes(&block->component_list.sprite_component.vertex_attribs, vertex_render_data, sizeof(Vertex)*arrlen(vertex_render_data), NULL, 0, false);
	init_shader_program(&block->component_list.sprite_component.shader_program, "shaders/block-vertex-shader.glsl", "shaders/block-fragment-shader.glsl");

	camera.position = (vec3s){{10.0f, 20.0f, 10.0f}};
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

	move_camera(&camera, input_state, game_engine.deltatime);
}

static void render()
{
#if defined(_PLATFORM_ANDROID)
	ui_render_joystick("Input", "Joystick", joystick_box_size, joystick_radius, joystick_color, &joystick_angle, &is_joystick_active);
#endif
	
	bind_texture(&block->component_list.sprite_component.texture);
	
	bind_shader_program(&block->component_list.sprite_component.shader_program);
	uniform_mat4(&block->component_list.sprite_component.shader_program, "projection", camera.projection_matrix);
	uniform_mat4(&block->component_list.sprite_component.shader_program, "view", camera.view_matrix);

	bind_vertex_buffer(&block->component_list.sprite_component.vertex_attribs, VAO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(*vertex_render_data), vertex_render_data);
	glDrawArrays(GL_TRIANGLES, 0, arrlen(vertex_render_data));
}

static void activate()
{
}

static void deactivate()
{
}

static void destroy()
{
	destroy_texture(&block->component_list.sprite_component.texture);
	destroy_shader_program(&block->component_list.sprite_component.shader_program);
	destroy_vertex_attributes(&block->component_list.sprite_component.vertex_attribs, false);
}


Scene SceneBlock = {"SceneBlock", init, destroy, activate, deactivate, update, render, process_input};
