#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdbool.h>

typedef struct
{
	bool key_pressed_data[GLFW_KEY_LAST];
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
void change_window_color(char colorcode[7], float alpha);
void destroy_window(Window* window);

