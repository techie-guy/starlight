#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <cglm/struct.h>

typedef struct
{
	bool key_pressed;
	bool key_pressed_data[GLFW_KEY_LAST];

	bool mouse_moved;
	vec2s mouse_position;

	bool mouse_clicked;
	bool mouse_clicked_data[GLFW_MOUSE_BUTTON_LAST];
} InputSystem;

typedef struct
{
	GLFWwindow* handle;
	unsigned int width;
	unsigned int height;
	const char* title;

	InputSystem input_system;
} Window;

void init_window(Window* window);
int should_window_close(Window* window);
void window_poll_events();
void window_swap_buffers(Window* window);
void change_window_color(vec4s color);
void destroy_window(Window* window);

