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

void initVertexAttributes(VertexAttributes* attrib, void* vertexData, size_t vertexDataSize, void* indexData, size_t indexDataSize)
{
	glGenVertexArrays(1, &attrib->VAO);
	bindBuffer(attrib, VAO);

	glGenBuffers(1, &attrib->VBO);
	bindBuffer(attrib, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexDataSize, vertexData, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &attrib->EBO);
	bindBuffer(attrib, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSize, indexData, GL_STATIC_DRAW);
}

void bindBuffer(VertexAttributes* attrib, enum Buffers buffer)
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

void unbindBuffer(VertexAttributes* attrib, enum Buffers buffer)
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

void destroyVertexAttributes(VertexAttributes* attrib)
{
	glDeleteVertexArrays(1, &attrib->VAO);
	glDeleteBuffers(1, &attrib->VBO);
	glDeleteBuffers(1, &attrib->EBO);
}
