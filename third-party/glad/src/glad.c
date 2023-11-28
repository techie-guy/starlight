#if defined(_PLATFORM_NATIVE)
	#define GLAD_GLES2_IMPLEMENTATION
	#include <glad/gles2.h>
#elif defined(_PLATFORM_WEB)
	#define GLFW_INCLUDE_ES2
	#include <GLFW/glfw3.h>
#endif
