#include "Window.h"

#if defined(_PLATFORM_DESKTOP)
	#include <glad/gles2.h>
#elif defined(_PLATFORM_WEB)
	#include <emscripten.h>
	#include <GLES3/gl3.h>
#elif defined(_PLATFORM_ANDROID)
	#include <GLES3/gl3.h>
	#include <android/native_window.h>
	#define GLFW_EXPOSE_NATIVE_ANDROID
	#include <GLFW/glfw3native.h>
#endif

#include "Utils.h"

static Window* current_window = NULL;

static void GLFW_error_callback(int errorcode, const char* error_description)
{
	log_error("GLFW Error: [Error Code]: %d, [Error Description]: %s\n", errorcode, error_description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	current_window->input_system.key_pressed = true;

	if(key >= 0 && key < GLFW_KEY_LAST)
	{
		current_window->input_system.key_pressed_data[key] = (action == GLFW_PRESS) || (action == GLFW_REPEAT);
	}
}

static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	current_window->input_system.mouse_moved = true;
	current_window->input_system.mouse_position = (vec2s){xpos, ypos};
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	current_window->input_system.mouse_clicked = true;

	if(button >= 0 && button < GLFW_MOUSE_BUTTON_LAST)
	{
		current_window->input_system.mouse_clicked_data[button] = (action == GLFW_PRESS) || (action == GLFW_REPEAT);
	}

}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	current_window->width = width;
	current_window->height = height;

	glViewport(0, 0, width, height);
}

void init_window(Window* window)
{
	current_window = window;

	glfwSetErrorCallback(GLFW_error_callback);

	glfwInit();
	
#if defined(_PLATFORM_ANDROID)
	glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
#endif
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	
	window->handle = glfwCreateWindow(window->width, window->height, window->title, NULL, NULL);
	if(!window->handle)
	{
		glfwTerminate();
		log_error("Failed to Initialize GLFW Window!\n");
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window->handle);
	glfwSwapInterval(1);

#if defined(_PLATFORM_DESKTOP)
	gladLoadGLES2(glfwGetProcAddress);
#endif

#if defined(_PLATFORM_ANDROID)
	window->width = ANativeWindow_getWidth(glfwGetAndroidApp()->window);
	window->height = ANativeWindow_getHeight(glfwGetAndroidApp()->window);
	glfwSetWindowSize(window->handle, window->width, window->height);
#endif
	
	glViewport(0, 0, window->width, window->height);

	glfwSetKeyCallback(window->handle, key_callback);
	glfwSetCursorPosCallback(window->handle, mouse_callback);
	glfwSetMouseButtonCallback(window->handle, mouse_button_callback);
	glfwSetFramebufferSizeCallback(window->handle, framebuffer_size_callback);
}

void window_poll_events()
{
	current_window->input_system.key_pressed = false;
	current_window->input_system.mouse_moved = false;
	glfwPollEvents();
}

void window_swap_buffers(Window* window)
{
	glfwSwapBuffers(window->handle);
}

int should_window_close(Window* window)
{
	return glfwWindowShouldClose(window->handle);
}

void change_window_color(vec4s color)
{
	glClearColor(color.r, color.g, color.b, color.a);
}

void destroy_window(Window* window)
{
	glfwDestroyWindow(window->handle);
	glfwTerminate();
}
