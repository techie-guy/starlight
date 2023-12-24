#include "ECS.h"
#include "Components.h"
#include "Utils.h"

#include <stb_ds.h>

static int total_entities = 0;

static Entity** entities = NULL;

Entity* addEntity(char* tag)
{
	Entity* e = malloc(sizeof(Entity));
	e->id = total_entities++;
	e->tag = tag;
	e->is_active = true;
	e->component_list.entity_id = e->id;

	arrput(entities, e);

	return e;
}

void destroyECS()
{
	for(int i = 0; i < arrlen(entities); i++)
	{
		free(entities[i]);
	}
}
