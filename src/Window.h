#pragma once

#include <GLFW/glfw3.h>

typedef struct
{
	GLFWwindow* handle;
	unsigned int width;
	unsigned int height;
	const char* title;
} Window;

void initWindow(Window* window);
int shouldWindowClose(Window* window);
void updateWindow(Window* window);
void setWindowKeyCallback(Window* window, GLFWkeyfun callback);
void changeWindowColor(char colorCode[7], float alpha);
void destroyWindow(Window* window);
