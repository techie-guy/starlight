#include "Utils.h"

char* read_file(const char* filepath, const char* mode)
{
#if !defined(_PLATFORM_ANDROID)
	if(access(filepath, F_OK) != 0)
	{
		log_error("File %s does not exist!\n", filepath);
		exit(EXIT_FAILURE);
	}
#endif

	char* buffer = NULL;
	long length;

	FILE* file = fopen(filepath, mode);

	if(file)
	{
		fseek(file, 0, SEEK_END);
		length = ftell(file);
		fseek(file, 0, SEEK_SET);

		buffer = malloc(length + 1);

		if(buffer)
		{
			fread(buffer, 1, length, file);
			buffer[length] = '\0';
		}

		fclose(file);
	}

	return buffer;
}

void write_to_file(const char* filepath, char* data)
{
	FILE* file = fopen(filepath, "w+");
	if(file)
	{
		fprintf(file, "%s", data);
		fclose(file);
	}
}

vec4s hex_to_rbg(char colorcode[7], float alpha)
{
	char hex_string[9] = {};

	hex_string[0] = '0';
	hex_string[1] = 'x';
	hex_string[8] = '\0';

	for(size_t i = 0; i < 6; i++)
	{
		hex_string[i+2] = colorcode[i+1];
	}
	
	int hexcode = strtol(hex_string, NULL, 16);

	return (vec4s){((hexcode >> 16) & 0xFF)/255.0f, ((hexcode >> 8) & 0xFF)/255.0f, ((hexcode) & 0xFF)/255.0f, alpha};
}
