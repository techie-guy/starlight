#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

// Color Codes taken from https://gist.github.com/iamnewton/8754917
#if defined(_PLATFORM_NATIVE)
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
#define log_debug(...) printf("%s[Debug]: %s", COLOR_BOLD_WHITE, COLOR_WHITE); printf(__VA_ARGS__); printf(COLOR_RESET)
#define log_info(...) printf("%s[Info]: %s", COLOR_BOLD_WHITE, COLOR_BLUE); printf(__VA_ARGS__); printf(COLOR_RESET)
#define log_warn(...) printf("%s[Warning]: %s", COLOR_BOLD_WHITE, COLOR_YELLOW); printf(__VA_ARGS__); printf(COLOR_RESET)
#define log_error(...) printf("%s[Error]: %s", COLOR_BOLD_WHITE, COLOR_RED); printf(__VA_ARGS__); printf(COLOR_RESET)

// Functions
char* readFile(const char* filepath);

#ifdef UTILS_DEF
char* readFile(const char* filepath)
{
	if(access(filepath, F_OK) != 0)
	{
		log_error("File %s does not exist!\n", filepath);
		exit(EXIT_FAILURE);
	}

	char* buffer = "";
	long length;

	FILE* file = fopen(filepath, "r");

	if(file)
	{
		fseek(file, 0, SEEK_END);
		length = ftell(file);
		fseek(file, 0, SEEK_SET);

		buffer = malloc(length + 1);

		if(buffer)
		{
			fread(buffer, 1, length, file);
		}

		buffer[length] = '\0';

		fclose(file);
	}

	return buffer;
}
#endif
