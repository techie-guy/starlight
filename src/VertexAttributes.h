#pragma once

#include <cglm/cglm.h>
#include <cglm/struct.h>

typedef struct
{
	vec3s position;
	vec4s color;
	vec2s texCoord;
} Vertex;

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

enum Buffers
{
	VAO,
	VBO,
	EBO
};

void initVertexAttributes(VertexAttributes* attrib, void* vertexData, size_t vertexDataSize, void* indexData, size_t indexDataSize);
void bindBuffer(VertexAttributes* attrib, enum Buffers buffer);
void unbindBuffer(VertexAttributes* attrib, enum Buffers buffer);
//void bindVAO(VertexAttributes* attrib);
//void bindVBO(VertexAttributes* attrib);
//void bindEBO(VertexAttributes* attrib);
void destroyVertexAttributes(VertexAttributes* attrib);
