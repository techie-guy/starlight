#pragma once

#include <cglm/struct.h>
#include "cimgui.h"

void init_imgui(const char* font_path, int font_size, const char* chars_add, const char* glsl_version);
void new_imgui_frame();
void render_imgui();

void ui_render_joystick(const char* joystick_box_tag, const char* joystick_circle_tag, ImVec2 joystick_box_size, float joystick_radius, ImVec4 joystick_color, float* joystick_angle, bool* is_joystick_active);

void destroy_imgui();
