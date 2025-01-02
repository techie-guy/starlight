#pragma once

#include "cgltf.h"

#include "Utils.h"
#include "Texture.h"

enum ModelType
{
	MODEL_GLTF = 1 << 0,
};


typedef struct
{
	cgltf_options options;
	cgltf_data* data;

	size_t vertex_count;
	size_t index_count;

	enum ModelType type;
} Model;

void model_load_from_file(Model* model, const char* filepath);
void model_parse_data(Model* model, Vertex** vertex_data, GLushort** index_data);
void model_load_texture(Model* model, Texture* texture);

void model_free(Model* model);
