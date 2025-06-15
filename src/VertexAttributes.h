#pragma once

#include "Utils.h"

#include <cglm/cglm.h>
#include <cglm/struct.h>

typedef struct
{
	Vertex vertices[4];
} Quad;

typedef struct
{
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
} VertexAttributes;

typedef struct { char* key; VertexAttributes value; } VertexAttribHashMap;

enum Buffers
{
	VAO,
	VBO,
	EBO
};

void init_vertex_attributes(VertexAttributes* attrib, void* vertex_data, size_t vertex_data_size, void* index_data, size_t index_data_size, bool indexed);
void bind_vertex_buffer(VertexAttributes* attrib, enum Buffers buffer);
void unbind_vertex_buffer(VertexAttributes* attrib, enum Buffers buffer);
void destroy_vertex_attributes(VertexAttributes* attrib, bool indexed);
