#pragma once

#include "Components.h"

typedef struct
{
	size_t id;
	bool is_active;
	char* tag;

	ComponentList component_list;
} Entity;

Entity* addEntity(char* tag);
void destroyECS();

