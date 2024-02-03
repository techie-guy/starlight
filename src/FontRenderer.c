#include "FontRenderer.h"
#include "ShaderProgram.h"
#include "VertexAttributes.h"
#include "Texture.h"
#include "Utils.h"

#if defined(_PLATFORM_DESKTOP)
	#include <glad/gles2.h>
#elif defined(_PLATFORM_WEB)
	#include <emscripten.h>
	#include <GLES3/gl3.h>
#elif defined(_PLATFORM_ANDROID)
	#include <GLES3/gl3.h>
#endif

#include <stb_truetype.h>
#include <stb_ds.h>

#define FONT_VERTEX_SHADER_PATH "shaders/font-vertex-shader.glsl"
#define FONT_FRAGMENT_SHADER_PATH "shaders/font-fragment-shader.glsl"

typedef struct
{
	Texture texture;
	vec2s size;
	vec2s offset;
	unsigned int advance;
} Character;

static struct { char key; Character value; }* characters = NULL;

static stbtt_fontinfo font;
static unsigned char* buffer;
static int font_character_size;

static unsigned int shader_program;
static VertexAttributes vertex_attributes;

static Window* current_window;

static GLushort indices[6] =
{
	0, 1, 2, 0, 2, 3
};

mat4s projection = GLMS_MAT4_IDENTITY_INIT;

void initFontRenderer(const char* font_path, int character_size, Window* window)
{
	current_window = window;

	initShaderProgram(&shader_program, FONT_VERTEX_SHADER_PATH, FONT_FRAGMENT_SHADER_PATH);	
	initVertexAttributes(&vertex_attributes, NULL, sizeof(Quad), indices, sizeof(indices));

	//  Loading the .ttf file
	buffer = (unsigned char*)read_file(font_path, "rb");
	stbtt_InitFont(&font, buffer, 0);
	font_character_size = character_size;
	float scale = stbtt_ScaleForPixelHeight(&font, character_size);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
	
	for (unsigned char c = 0; c < 128; c++)
	{
		int glyph_index = stbtt_FindGlyphIndex(&font, c);
		int width, height, x_offset, y_offset;
		unsigned char* bitmap = stbtt_GetGlyphBitmap(&font, 0, scale, glyph_index, &width, &height, &x_offset, &y_offset);

		// generate texture
		Texture texture;
		texture.width = width;
		texture.height = height;
		initTextureFromData(&texture, 0, GL_R8, GL_RED, GL_UNSIGNED_BYTE, bitmap);

		// set texture options
		setTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    	setTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    	setTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    setTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
		// now store character for later use
	    Character character = 
		{
    	    texture, 
			(vec2s){width, height},
			(vec2s){x_offset, y_offset},
    	    stbtt_GetGlyphKernAdvance(&font, glyph_index, stbtt_FindGlyphIndex(&font, c+1))
    	};
		hmput(characters, c, character);
	}
	unbindTexture();
}

// BUG: Blurry text if scale is not 1.0
// BUG: Text not aligned properly
void renderText(char* text, float x, float y, float scale, vec3s color)
{
	bindShaderProgram(&shader_program);
	projection = glms_ortho(0, current_window->width, 0, current_window->height, -1.0f, 1.0f);
	uniformMat4(&shader_program, "projection", projection);
	uniformVec3(&shader_program, "textColor", color);

	bindBuffer(&vertex_attributes, VAO);

	for(char* c = text; *c != '\0'; c++)
	{
		Character ch = hmget(characters, *c);

		float xpos = x + ch.offset.x;
        float ypos = y + ch.offset.y;

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
		bindBuffer(&vertex_attributes, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad), quad);
        // render quad
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
        x += w;
	}

	unbindBuffer(&vertex_attributes, VAO);
	unbindTexture();
}
