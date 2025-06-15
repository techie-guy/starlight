#include "ECS.h"
#include "Components.h"
#include "Utils.h"

#include <stb_ds.h>

static int total_entities = 0;

static Entity** entities = NULL;

Entity* add_entity(char* tag)
{
	Entity* e = malloc(sizeof(Entity));
	e->id = total_entities++;
	e->tag = tag;
	e->is_active = true;

	e->component_list.sprite_component.textures = NULL;

	arrput(entities, e);

	return e;
}

void destroy_ECS()
{
	for(int i = 0; i < arrlen(entities); i++)
	{
		free(entities[i]);
	}
}
