#include "VertexAttributes.h"
#include "Utils.h"

#if defined(_PLATFORM_DESKTOP)
	#include <glad/gles2.h>
#elif defined(_PLATFORM_WEB)
	#include <emscripten.h>
	#include <GLES3/gl3.h>
#elif defined(_PLATFORM_ANDROID)
	#include <GLES3/gl3.h>
#endif

void init_vertex_attributes(VertexAttributes* attrib, void* vertex_data, size_t vertex_data_size, void* index_data, size_t index_data_size, bool indexed)
{
	glGenVertexArrays(1, &attrib->VAO);
	bind_vertex_buffer(attrib, VAO);

	glGenBuffers(1, &attrib->VBO);
	bind_vertex_buffer(attrib, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertex_data_size, vertex_data, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(3);

	if(indexed)
	{
		glGenBuffers(1, &attrib->EBO);
		bind_vertex_buffer(attrib, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_data_size, index_data, GL_DYNAMIC_DRAW);
	}
}

void bind_vertex_buffer(VertexAttributes* attrib, enum Buffers buffer)
{
	switch(buffer)
	{
		case VAO:
			glBindVertexArray(attrib->VAO);
		case VBO:
			glBindBuffer(GL_ARRAY_BUFFER, attrib->VBO);
		case EBO:
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, attrib->EBO);
	}
}

void unbind_vertex_buffer(VertexAttributes* attrib, enum Buffers buffer)
{
	switch(buffer)
	{
		case VAO:
			glBindVertexArray(0);
		case VBO:
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		case EBO:
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void destroy_vertex_attributes(VertexAttributes* attrib, bool indexed)
{
	glDeleteVertexArrays(1, &attrib->VAO);
	glDeleteBuffers(1, &attrib->VBO);

	if(indexed)
	{
		glDeleteBuffers(1, &attrib->EBO);
	}
}
