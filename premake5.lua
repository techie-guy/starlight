require("third-party/premake5-emcc/emscripten")
require("third-party/premake-androidndk/androidndk")

workspace "mist-lib"
	startproject "mist-lib"

	configurations { "Debug", "Release" }
	platforms { "Desktop", "Web", "Android" }

	filter { "platforms:Desktop" }
		toolset "clang"
	filter { "platforms:Web" }
	    toolset "emcc"

	outputdir = "%{cfg.platform}-%{cfg.buildcfg}"

	window_width = 1280
	window_height = 720
	window_title = "Mist Lib"
	assets_dir = "assets"

project "mist-lib"	
	kind "ConsoleApp"
	language "C"
	targetdir "bin/%{outputdir}/%{prj.name}"
	objdir "bin/%{outputdir}/%{prj.name}/obj"
	location "build"

	filter { "platforms:Desktop" }
		assets_dir = "assets"

		prebuildcommands
		{
			"{COPYDIR} ../%{assets_dir} %{cfg.buildtarget.directory}"
		}

		defines
		{
			"_PLATFORM_DESKTOP",
		}

		links
		{
			"freetype",
		}

		includedirs
		{
			"/usr/include/freetype2",
			"/usr/include/libpng16"
		}
	filter{}

	filter { "platforms:Android" }
		assets_dir = ""

		system "android"
		architecture "ARM64"
		systemversion "30"
		kind "SharedLib"

		prebuildcommands
		{
			"{COPYDIR} assets android-project/app/src/main",
		}

		links
		{
			"android_fopen"
		}

		defines
		{
			"_PLATFORM_ANDROID",
		}

		buildoptions
		{
			"-include src/android-fopen.h"
		}

	filter{}

	filter { "platforms:Web" }
		assets_dir = "assets"

		targetextension ".html"

		defines
		{
			"_PLATFORM_WEB",
		}

		links
		{
			"glfw3",
			"freetype",
		}

		buildoptions
		{
			"-sUSE_FREETYPE=1",
		}

		linkoptions
		{
			"-sUSE_GLFW=3",
			"-sWASM=1",
			"-sMAX_WEBGL_VERSION=2",
			"-sALLOW_MEMORY_GROWTH=1",
			"-sNO_EXIT_RUNTIME=0",
			"-sASSERTIONS=1",
			"--preload-file %{assets_dir}",
			"--shell-file %{assets_dir}/index.html",
		}
	filter {}

	defines
	{
		"WINDOW_WIDTH=%{window_width}",
		"WINDOW_HEIGHT=%{window_height}",
		"WINDOW_TITLE=\"%{window_title}\"",
		"ASSETS_DIR=\"%{assets_dir}\"",
	}

	links
	{
		"m",
		"stdc++",
		"cimgui",
		"glfw",
	}

	includedirs
	{
		"third-party/glfw/include",
		"third-party/cglm/include",
		"third-party/stb/include",
		"third-party/glad/include",
		"third-party/hashmap.c",
		"third-party/cJSON",
		"third-party/cimgui",
		"third-party/cimgui/imgui",
		"third-party/cimgui/imgui/backends",
	}

	files 
	{ 
		"src/**.h", "src/**.c",
		"third-party/cglm/src/**.c", "third-party/cglm/include/**.h",
		"third-party/cJSON/cJSON.c", "third-party/cJSON/cJSON.h",
		"third-party/hashmap.c/hashmap.c", "third-party/hashmap.c/hashmap.h",
		"third-party/glad/src/glad.c", "third-party/glad/include/glad/gles2.h",
		"third-party/stb/src/*.c", "third-party/stb/include/*.h",	
	}

	removefiles
	{
		"src/android-fopen.h", "src/android-fopen.c"
	}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

filter { "platforms:Android" }
	project "android_native_app_glue"
		kind "StaticLib"
		location "build"
		targetdir "bin/%{outputdir}/%{prj.name}"
		objdir "bin/%{outputdir}/%{prj.name}/obj"
		architecture "ARM64"
		systemversion "30"

		filter { "platforms:Android" }
		files 
		{
			"$(ANDROID_NDK_HOME)/sources/android/native_app_glue/Android.mk",
		}

		links
		{
			"log",
			"android"
		}

		linkoptions
		{
			"-u ANativeActivity_onCreate"
		}
		filter{}

	project "android_fopen"
		kind "StaticLib"
		location "build"
		targetdir "bin/%{outputdir}/%{prj.name}"
		objdir "bin/%{outputdir}/%{prj.name}/obj"
		architecture "ARM64"
		systemversion "30"

		filter { "platforms:Android" }
		includedirs
		{
			"third-party/glfw/include",
		}

		defines
		{
			"_PLATFORM_ANDROID",
			"__USE_BSD",
		}

		files 
		{
			"src/android-fopen.h",
			"src/android-fopen.c",
		}

		links
		{
			"log",
			"android",
			"glfw"
		}
		filter{}
filter{}
	

project "glfw"
	kind "StaticLib"
	language "C"
	targetdir "bin/%{outputdir}/%{prj.name}"
	objdir "bin/%{outputdir}/%{prj.name}/obj"
	location "build"

	includedirs
	{
		"third-party/glfw/include"
	}	

	filter { "system:linux or system:android" }
		files
		{
			"third-party/glfw/src/posix_time.h",
			"third-party/glfw/src/posix_thread.h",
			"third-party/glfw/src/posix_module.c",
			"third-party/glfw/src/posix_time.c",
			"third-party/glfw/src/posix_thread.c",
			"third-party/glfw/src/posix_poll.h",
			"third-party/glfw/src/posix_poll.c",
		}
	filter {}

	filter { "platforms:Desktop" }
	filter { "system:linux" }
		local session_type = os.getenv("XDG_SESSION_TYPE")

		if session_type == "x11" then
			files
			{
				"third-party/glfw/src/x11_platform.h",
				"third-party/glfw/src/xkb_unicode.h",
				"third-party/glfw/src/x11_init.c",
				"third-party/glfw/src/x11_monitor.c",
				"third-party/glfw/src/x11_window.c",
				"third-party/glfw/src/xkb_unicode.c",
				"third-party/glfw/src/glx_context.c",
				"third-party/glfw/src/linux_joystick.h",
				"third-party/glfw/src/linux_joystick.c",
			}

			defines
			{
				"_GLFW_X11",
			}
		elseif session_type == "wayland" then
			files
			{
				"third-party/glfw/src/wl_platform.h",
				"third-party/glfw/src/xkb_unicode.h",
				"third-party/glfw/src/wl_init.c",
				"third-party/glfw/src/wl_monitor.c",
				"third-party/glfw/src/wl_window.c",
				"third-party/glfw/src/xkb_unicode.c",
				"third-party/glfw/src/linux_joystick.h",
				"third-party/glfw/src/linux_joystick.c",
			}

			defines
			{
				"_GLFW_WAYLAND",
			}
		end
	filter {}
	filter {}

	filter { "platforms:Android" }
		architecture "ARM64"
		systemversion "30"

		links
		{
			"android_native_app_glue",
			"EGL",
			"GLESv1_CM",
			"GLESv2",
			"GLESv3",
		}
		files
		{
			"third-party/glfw/src/android_platform.h",
			"third-party/glfw/src/android_init.c",
			"third-party/glfw/src/android_monitor.c",
			"third-party/glfw/src/android_window.c",
			"$(ANDROID_NDK_HOME)/sources/android/native_app_glue/android_native_app_glue.c"
		}

		defines
		{
			"_GLFW_ANDROID",
		}
	filter {}

	files
	{
		"third-party/glfw/include/GLFW/glfw3.h",
		"third-party/glfw/include/GLFW/glfw3native.h",
		"third-party/glfw/src/internal.h",
		"third-party/glfw/src/platform.h",
		"third-party/glfw/src/mappings.h",
		"third-party/glfw/src/context.c",
		"third-party/glfw/src/init.c",
		"third-party/glfw/src/input.c",
		"third-party/glfw/src/monitor.c",
		"third-party/glfw/src/platform.c",
		"third-party/glfw/src/vulkan.c",
		"third-party/glfw/src/window.c",
		"third-party/glfw/src/egl_context.c",
		"third-party/glfw/src/osmesa_context.c",
		"third-party/glfw/src/null_platform.h",
		"third-party/glfw/src/null_joystick.h",
		"third-party/glfw/src/null_init.c",
		"third-party/glfw/src/null_monitor.c",
		"third-party/glfw/src/null_window.c",
		"third-party/glfw/src/null_joystick.c",
	}

project "cimgui"
	kind "StaticLib"
	language "C++"
	targetdir "bin/%{outputdir}/%{prj.name}"
	objdir "bin/%{outputdir}/%{prj.name}/obj"
	location "build"

	filter { "platforms:Android" }
		buildoptions
		{
			"-include ../../src/android-fopen.h"
		}

		defines
		{
			"_PLATFORM_ANDROID"
		}

		links
		{
			"android_fopen",
		}
	filter{}

	includedirs
	{
		"third-party/glfw/include",
		"third-party/cimgui/imgui",
		"third-party/cimgui/imgui/backends",
	}

	files
	{
		"third-party/cimgui/cimgui.h",
		"third-party/cimgui/cimgui.cpp",
		"third-party/cimgui/cimgui_impl_opengl3.h",
		"third-party/cimgui/cimgui_impl_opengl3.cpp",
		"third-party/cimgui/cimgui_impl_glfw.h",
		"third-party/cimgui/cimgui_impl_glfw.cpp",
		"third-party/cimgui/imgui/imgui.cpp",
		"third-party/cimgui/imgui/imgui_demo.cpp",
		"third-party/cimgui/imgui/imgui_draw.cpp",
		"third-party/cimgui/imgui/imgui_tables.cpp",
		"third-party/cimgui/imgui/imgui_widgets.cpp",
		"third-party/cimgui/imgui/imconfig.h",
		"third-party/cimgui/imgui/backends/imgui_impl_opengl3.h",
		"third-party/cimgui/imgui/backends/imgui_impl_opengl3.cpp",
		"third-party/cimgui/imgui/backends/imgui_impl_glfw.h",
		"third-party/cimgui/imgui/backends/imgui_impl_glfw.cpp",
	}

