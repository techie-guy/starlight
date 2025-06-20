#pragma once

#include "renderer.h"

#include <cglm/struct.h>

typedef struct
{
	vec3s position;
	vec3s scale;
	float speed;

	mat4s transform;
} Component_Transform;

typedef struct
{
	VertexAttributes vertex_attribs;
	VertexAttribHashMap* vertex_attrib_hashmap;
	unsigned int shader_program;

	TextureHashMap* textures;
} Component_Sprite;

typedef struct
{
	Component_Transform transform_component;
	Component_Sprite sprite_component;
} ComponentList;
