#pragma once

#include <cglm/cglm.h>
#include <cglm/struct.h>

void init_shader_program(unsigned int* shader_program, const char* vertex_shader_source_path, const char* fragment_shader_source_path);
void bind_shader_program(unsigned int* shader_program);

int get_uniform_location(unsigned int* shader_program, const char* uniform_name);

void uniform_int(unsigned int* shader_program, const char* uniform_name, int data);
void uniform_float(unsigned int* shader_program, const char* uniform_name, float data);
void uniform_vec2(unsigned int* shader_program, const char* uniform_name, vec2s data);
void uniform_vec3(unsigned int* shader_program, const char* uniform_name, vec3s data);
void uniform_vec4(unsigned int* shader_program, const char* uniform_name, vec4s data);
void uniform_mat3(unsigned int* shader_program, const char* uniform_name, mat3s data);
void uniform_mat4(unsigned int* shader_program, const char* uniform_name, mat4s data);

void destroy_shader_program(unsigned int* shader_program);
