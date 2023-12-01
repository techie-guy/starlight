#pragma once

#include <cglm/struct.h>

void initFontRenderer(const char* fontPath, int characterSize);
void renderText(char* text, float x, float y, float scale, vec3s color);
