#if defined(_PLATFORM_DESKTOP)
	#define GLAD_GLES2_IMPLEMENTATION
	#include <glad/gles2.h>
#elif defined(_PLATFORM_WEB) || defined(_PLATFORM_ANDROID)
	#define GLFW_INCLUDE_ES3
	#include <GLFW/glfw3.h>
#endif
