#include "FontRenderer.h"
#include "Application.h"
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

void init_font_renderer(const char* font_path, int character_size)
{
	current_window = &game_engine.current_window;

	init_shader_program(&shader_program, FONT_VERTEX_SHADER_PATH, FONT_FRAGMENT_SHADER_PATH);
	init_vertex_attributes(&vertex_attributes, NULL, sizeof(Quad), indices, sizeof(indices), true);

	//  Loading the .ttf file
	buffer = (unsigned char*)read_file(font_path, "rb");
	stbtt_InitFont(&font, buffer, 0);
	font_character_size = character_size;
	float scale = stbtt_ScaleForPixelHeight(&font, character_size);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
	
	for (unsigned char c = 0; c < 128; c++)
	{
		int glyph_index = stbtt_FindGlyphIndex(&font, c);
		int width, height;
		unsigned char* bitmap = stbtt_GetGlyphBitmap(&font, 0, scale, glyph_index, &width, &height, NULL, NULL);

		// generate texture
		Texture texture;
		texture.width = width;
		texture.height = height;
		init_texture_from_data(&texture, 0, GL_R8, GL_RED, GL_UNSIGNED_BYTE, bitmap);

		// set texture options
		set_texture_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    	set_texture_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    	set_texture_parameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    set_texture_parameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
		// now store character for later use
	    Character character = 
		{
    	    texture, 
			(vec2s){width, height},
    	    stbtt_GetGlyphKernAdvance(&font, glyph_index, stbtt_FindGlyphIndex(&font, c+1))
    	};
		hmput(characters, c, character);
	}
	unbind_texture();
}

// BUG: Blurry text if scale is not 1.0
void render_text(char* text, float x, float y, float scale, char* hex_color, float opacity)
{
	bind_shader_program(&shader_program);
	projection = glms_ortho(0, current_window->width, 0, current_window->height, -1.0f, 1.0f);
	uniform_mat4(&shader_program, "projection", projection);
	uniform_vec4(&shader_program, "text_color", hex_to_rbg(hex_color, opacity));

	bind_vertex_buffer(&vertex_attributes, VAO);

	for(char* c = text; *c != '\0'; c++)
	{
		Character ch = hmget(characters, *c);

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
				{{x, y + h, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
				{{x, y, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
				{{x + w, y, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
				{{x + w, y + h, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}
				}
			}
		};

        // render glyph texture over quad
		bind_texture(&ch.texture);
        // update content of VBO memory
		bind_vertex_buffer(&vertex_attributes, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad), quad);
        // render quad
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
        x += w;
	}

	unbind_vertex_buffer(&vertex_attributes, VAO);
	unbind_texture();
}
