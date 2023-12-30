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

void initWindow(Window* window);
int shouldWindowClose(Window* window);
void windowPollEvents();
void windowSwapBuffers(Window* window);
void changeWindowColor(char colorCode[7], float alpha);
void destroyWindow(Window* window);

