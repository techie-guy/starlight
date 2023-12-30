#if defined(_PLATFORM_NATIVE)
	#include <glad/gles2.h>
#elif defined(_PLATFORM_WEB)
	#include <GLES3/gl3.h>
#endif

#include "Window.h"
#include "Utils.h"

static Window* current_window = NULL;

static void GLFW_error_callback(int errorCode, const char* errorDescription)
{
	log_error("GLFW Error: [Error Code]: %d, [Error Description]: \n%s\n", errorCode, errorDescription);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(key >= 0 && key < GLFW_KEY_LAST)
	{
		current_window->input_system.key_pressed_data[key] = (action == GLFW_PRESS) || (action == GLFW_REPEAT);
	}
}

void initWindow(Window* window)
{
	current_window = window;

	glfwSetErrorCallback(GLFW_error_callback);

	glfwInit();
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

	window->handle = glfwCreateWindow(window->width, window->height, window->title, NULL, NULL);
	if(!window->handle)
	{
		glfwTerminate();
		log_error("Failed to Initialize GLFW Window!\n");
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window->handle);
	glfwSwapInterval(1);

	#if defined(_PLATFORM_NATIVE)
		gladLoadGLES2(glfwGetProcAddress);
	#endif

	glfwSetKeyCallback(window->handle, key_callback);
}

void windowPollEvents()
{
	glfwPollEvents();
}

void windowSwapBuffers(Window* window)
{
	glfwSwapBuffers(window->handle);
}

int shouldWindowClose(Window* window)
{
	return glfwWindowShouldClose(window->handle);
}

void changeWindowColor(char colorCode[7], float alpha)
{
	char hexString[9] = {};

	hexString[0] = '0';
	hexString[1] = 'x';
	hexString[8] = '\0';

	for(size_t i = 0; i < 6; i++)
	{
		hexString[i+2] = colorCode[i+1];
	}

	int hexCode = strtol(hexString, NULL, 16);

	glClearColor(((hexCode >> 16) & 0xFF)/255.0f, ((hexCode >> 8) & 0xFF)/255.0f, ((hexCode) & 0xFF)/255.0f, alpha);
}

void destroyWindow(Window* window)
{
	glfwDestroyWindow(window->handle);
	glfwTerminate();
}
