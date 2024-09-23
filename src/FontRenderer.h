#pragma once

#include <cglm/struct.h>

void init_font_renderer(const char* font_path, int character_size);
//void render_text(char* text, float x, float y, float scale, char* hex_color, float opacity);
//void render_text(char* text, vec3s position, float scale, char* hex_color, float opacity, mat4s projection, mat4s view);
void render_text(char* text, float x, float y, float scale, char* hex_color, float opacity, mat4s projection, mat4s view);
