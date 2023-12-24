#pragma once

#include <cglm/struct.h>

typedef struct
{
	vec3s position;
	vec3s scale;
} Component_Transform;

typedef struct
{
	int entity_id;
	Component_Transform transform_component;
} ComponentList;
