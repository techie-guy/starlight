#pragma once

#include "cgltf.h"

#include "Utils.h"
#include "Texture.h"
#include "VertexAttributes.h"

#include <stb_ds.h>

enum ModelType
{
	MODEL_GLTF = 1 << 0,
};

typedef struct
{
	cgltf_material* gltf_material;
	TextureHashMap* texture_hashmap;

	float metallic_factor;
	float roughness_factor;
} Material;

typedef struct
{
	size_t vertex_count;
	size_t index_count;

	VertexAttributes vertex_attribs;
	
	Vertex* vertex_render_data;
	GLushort* index_data;

	cgltf_mesh* mesh;

	Material material;
} Mesh;

typedef struct
{
	cgltf_options options;
	cgltf_data* data;

	size_t vertex_count;
	size_t index_count;

	struct { char* key; Mesh value; }* meshes_hashmap;

	enum ModelType type;
} Model;

void model_load_from_file(Model* model, const char* filepath);
//void model_parse_data(Model* model, Vertex** vertex_data, GLushort** index_data);
//void model_parse_data(Model* model);
void model_parse_data(Model* model);
//void model_load_materials(Model* model, TextureHashMap** textures);

void model_free(Model* model);
