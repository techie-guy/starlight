#pragma once

#include <cglm/struct.h>

void init_font_renderer(const char* font_path, int character_size);
void render_text(char* text, float x, float y, float scale, char* hex_color, float opacity);
