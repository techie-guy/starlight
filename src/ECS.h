#pragma once

#include "Components.h"

typedef struct
{
	size_t id;
	bool is_active;
	char* tag;

	ComponentList component_list;
} Entity;

Entity* add_entity(char* tag);
void destroy_ECS();

