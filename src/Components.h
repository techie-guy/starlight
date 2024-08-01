#pragma once

#include "Texture.h"
#include "VertexAttributes.h"

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
	unsigned int shader_program;
	Texture texture;
} Component_Sprite;

typedef struct
{
	Component_Transform transform_component;
	Component_Sprite sprite_component;
} ComponentList;
