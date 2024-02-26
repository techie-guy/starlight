#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include <cglm/struct.h>

#if defined(_PLATFORM_ANDROID)
	#include <android/log.h>
	#define LOG(...) ((void)__android_log_print(ANDROID_LOG_INFO, "MIST_LIB", __VA_ARGS__))	
#else
	#define LOG printf
#endif

typedef struct
{
	bool left;
	bool right;
	bool up;
	bool down;
	bool space;
	bool l_ctrl;
	bool keyPress;
} InputState;

// Color Codes taken from https://gist.github.com/iamnewton/8754917
#if defined(_PLATFORM_DESKTOP)
  #define COLOR_RESET       "\e[0m"
  #define COLOR_RED         "\e[0;31m"
  #define COLOR_GREEN       "\e[0;32m"
  #define COLOR_YELLOW      "\e[0;33m"
  #define COLOR_BLUE        "\e[0;34m"
  #define COLOR_WHITE       "\e[0;37m"
  #define COLOR_BOLD_RED    "\e[1;31m"
  #define COLOR_BOLD_GREEN  "\e[1;32m"
  #define COLOR_BOLD_YELLOW "\e[1;33m"
  #define COLOR_BOLD_BLUE   "\e[1;34m"
  #define COLOR_BOLD_WHITE  "\e[1;37m"
#else
  #define COLOR_RESET ""
  #define COLOR_RED ""
  #define COLOR_GREEN ""
  #define COLOR_YELLOW ""
  #define COLOR_BLUE ""
  #define COLOR_WHITE ""
  #define COLOR_BOLD_RED ""
  #define COLOR_BOLD_GREEN ""
  #define COLOR_BOLD_YELLOW ""
  #define COLOR_BOLD_BLUE ""
  #define COLOR_BOLD_WHITE ""
#endif

// Printing Macros
#define log_debug(...) LOG("%s[Debug]: %s", COLOR_BOLD_WHITE, COLOR_WHITE); LOG(__VA_ARGS__); LOG(COLOR_RESET)
#define log_info(...) LOG("%s[Info]: %s", COLOR_BOLD_BLUE, COLOR_WHITE); LOG(__VA_ARGS__); LOG(COLOR_RESET)
#define log_warn(...) LOG("%s[Warning]: %s", COLOR_BOLD_YELLOW, COLOR_WHITE); LOG(__VA_ARGS__); LOG(COLOR_RESET)
#define log_error(...) LOG("%s[Error]: %s", COLOR_BOLD_RED, COLOR_WHITE); LOG(__VA_ARGS__); LOG(COLOR_RESET)

// Functions
char* read_file(const char* filepath, const char* mode);
vec4s hex_to_rbg(char colorcode[7], float alpha);
