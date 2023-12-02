#include "FontRenderer.h"

#if defined(_PLATFORM_NATIVE)
	#include <glad/gles2.h>
#elif defined(_PLATFORM_WEB)
	#include <GLES3/gl3.h>
#endif

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

#include <stb_ds.h>

#include "ShaderProgram.h"
#include "VertexAttributes.h"
#include "Texture.h"

#define FONT_VERTEX_SHADER_PATH "assets/shaders/font-vertex-shader.glsl"
#define FONT_FRAGMENT_SHADER_PATH "assets/shaders/font-fragment-shader.glsl"

typedef struct
{
//	unsigned int textureID;
	Texture texture;
	vec2s size;
	vec2s bearing;
	unsigned int advance;
} Character;

static struct { char key; Character value; }* characters = NULL;

static FT_Library ft;
static FT_Face face;

static unsigned int shaderProgram;
static VertexAttributes vertexAttributes;

static GLushort indices[6] =
{
	0, 1, 2, 0, 2, 3
};

mat4s projection = GLMS_MAT4_IDENTITY_INIT;

void initFontRenderer(const char* fontPath, int characterSize)
{
	initShaderProgram(&shaderProgram, FONT_VERTEX_SHADER_PATH, FONT_FRAGMENT_SHADER_PATH);
	projection = glms_ortho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1.0f, 1.0f);
	
	bindShaderProgram(&shaderProgram);
	uniformMat4(&shaderProgram, "projection", projection);

	initVertexAttributes(&vertexAttributes, NULL, sizeof(Quad), indices, sizeof(indices));

	//  Loading the .ttf file
	FT_Init_FreeType(&ft);
	FT_New_Face(ft, fontPath, 0, &face);
	FT_Set_Pixel_Sizes(face, 0, characterSize);  

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

	for (unsigned char c = 0; c < 128; c++)
	{
		// load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			printf("Failed to load Glyph!\n");
			continue;
		}

		// generate texture
		Texture texture;
		texture.width = face->glyph->bitmap.width;
		texture.height = face->glyph->bitmap.rows;
		initTextureFromData(&texture, 0, GL_RED, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		// set texture options
		setTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    	setTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    	setTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    setTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
		// now store character for later use
	    Character character = 
		{
    	    texture, 
			(vec2s){face->glyph->bitmap.width, face->glyph->bitmap.rows},
			(vec2s){face->glyph->bitmap_left, face->glyph->bitmap_top},
    	    face->glyph->advance.x
    	};
		hmput(characters, c, character);	
	}
	unbindTexture();

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void renderText(char* text, float x, float y, float scale, vec3s color)
{
	bindShaderProgram(&shaderProgram);
	uniformVec3(&shaderProgram, "textColor", color);
	bindBuffer(&vertexAttributes, VAO);

	for(char* c = text; *c != '\0'; c++)
	{
		Character ch = hmget(characters, *c);

		float xpos = x + ch.bearing.x * scale;
        float ypos = y - (ch.size.y - ch.bearing.y) * scale;

        float w = ch.size.x * scale;
        float h = ch.size.y * scale;

        // update VBO for each character
		/*
			Top Left: 0
			Bottom Left: 1
			Bottom Right: 2
			Top Right: 3

			0 1 2 0 2 3
		*/

		Quad quad[1] =
		{
			(Quad)
			{
				{
				{{xpos, ypos + h, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
				{{xpos, ypos, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
				{{xpos + w, ypos, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
				{{xpos + w, ypos + h, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}
				}
			}
		};

        // render glyph texture over quad
		bindTexture(&ch.texture);
        // update content of VBO memory
		bindBuffer(&vertexAttributes, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad), quad);
        // render quad
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}

	unbindBuffer(&vertexAttributes, VAO);
	unbindTexture();
}
