#pragma once

#include <cglm/struct.h>

void packi16(unsigned char* buf, unsigned int i);
void packi32(unsigned char* buf, unsigned long int i);
void packi64(unsigned char* buf, unsigned long long int i);
void packvec2s(unsigned char* buf, vec2s v);
void packvec3s(unsigned char* buf, vec3s v);
void packvec4s(unsigned char* buf, vec4s v);
void packmat4s(unsigned char* buf, mat4s m);

int unpacki16(unsigned char* buf);
unsigned int unpacku16(unsigned char* buf);
long int unpacki32(unsigned char* buf);
unsigned long int unpacku32(unsigned char* buf);
long long int unpacki64(unsigned char* buf);
unsigned long long int unpacku64(unsigned char* buf);
vec2s unpackvec2s(unsigned char* buf);
vec3s unpackvec3s(unsigned char* buf);
vec4s unpackvec4s(unsigned char* buf);
mat4s unpackmat4s(unsigned char* buf);


unsigned int pack(unsigned char* buf, char* format, ...);
void unpack(unsigned char* buf, char* format, ...);
