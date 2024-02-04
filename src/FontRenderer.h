#pragma once

#include <cglm/struct.h>
#include "Window.h"

void init_font_renderer(const char* font_path, int character_size, Window* window);
void render_text(char* text, float x, float y, float scale, vec3s color);
