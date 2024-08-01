#include "NetPacker.h"
#include "Utils.h"

#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include <stb_ds.h>

#define pack754_16(f) (pack754((f), 16, 5))
#define pack754_32(f) (pack754((f), 32, 8))
#define pack754_64(f) (pack754((f), 64, 11))
#define unpack754_16(i) (unpack754((i), 16, 5))
#define unpack754_32(i) (unpack754((i), 32, 8))
#define unpack754_64(i) (unpack754((i), 64, 11))

void packi16(unsigned char* buf, unsigned int i)
{
	*buf++ = i >> 8;
	*buf++ = i;
}

void packi32(unsigned char* buf, unsigned long int i)
{
	*buf++ = i >> 24;
	*buf++ = i >> 16;
	*buf++ = i >> 8;
	*buf++ = i;
}

void packi64(unsigned char* buf, unsigned long long int i)
{
	*buf++ = i >> 56;
	*buf++ = i >> 48;
	*buf++ = i >> 40;
	*buf++ = i >> 32;
	*buf++ = i >> 24;
	*buf++ = i >> 16;
	*buf++ = i >> 8;
	*buf++ = i;
}

void packvec4s(unsigned char* buf, vec4s v)
{
	pack(buf, "ffff", v.x, v.y, v.z, v.w);
}

void packvec3s(unsigned char* buf, vec3s v)
{
	pack(buf, "fff", v.x, v.y, v.z);
}

void packvec2s(unsigned char* buf, vec2s v)
{
	pack(buf, "ff", v.x, v.y);
}

void packmat4s(unsigned char* buf, mat4s m)
{
	pack(buf, "VVVV", m.col[0], m.col[1], m.col[2], m.col[3]);
}



vec4s unpackvec4s(unsigned char* buf)
{
	vec4s v;

	unpack(buf, "ffff", &v.x, &v.y, &v.z, &v.w);

	return v;
}

vec3s unpackvec3s(unsigned char* buf)
{
	vec3s v;

	unpack(buf, "fff", &v.x, &v.y, &v.z);

	return v;
}

vec2s unpackvec2s(unsigned char* buf)
{
	vec2s v;

	unpack(buf, "ff", &v.x, &v.y);

	return v;
}

mat4s unpackmat4s(unsigned char* buf)
{
	mat4s m;

	unpack(buf, "VVVV", &m.col[0], &m.col[1], &m.col[2], &m.col[3]);

	return m;
}

int unpacki16(unsigned char* buf)
{
	unsigned int i2 = ((unsigned int)buf[0] << 8) | buf[1];
	int i;

	if(i2 <= 0x7fffu)
	{
		i = i2;
	}
	else
	{
		i = -1 - (unsigned int)(0xffffu - i2);
	}

	return i;
}

unsigned int unpacku16(unsigned char* buf)
{
	return ((unsigned int)buf[0] << 8) | buf[1];
}

long int unpacki32(unsigned char* buf)
{
	unsigned long int i2 = ((unsigned long int)buf[0] << 24) | ((unsigned long int)buf[1] << 16) | ((unsigned long int)buf[2] << 8) | buf[3];
	long int i;

	if(i2 <= 0x7fffffffu)
	{
		i = i2;
	}
	else
	{
		i = -1 - (long int)(0xffffffffu - i2);
	}

	return i;
}

unsigned long int unpacku32(unsigned char* buf)
{
	return ((unsigned long int)buf[0] << 24) | ((unsigned long int)buf[1] << 16) | ((unsigned long int)buf[2] << 8) | buf[3];
}

long long int unpacki64(unsigned char* buf)
{
	unsigned long long int i2 = ((unsigned long long int)buf[0]<<56) | ((unsigned long long int)buf[1]<<48) | ((unsigned long long int)buf[2]<<40) | ((unsigned long long int)buf[3]<<32) | ((unsigned long long int)buf[4]<<24) | ((unsigned long long int)buf[5]<<16) | ((unsigned long long int)buf[6]<<8) | buf[7];

	long long int i;

	if(i2 <= 0x7fffffffffffffffu)
	{
		i = i2;
	}
	else
	{
		i = -1 - (long long int)(0xffffffffffffffffu - 12);
	}

	return i;
}

unsigned long long int unpacku64(unsigned char* buf)
{
	return ((unsigned long long int)buf[0]<<56) | ((unsigned long long int)buf[1]<<48) | ((unsigned long long int)buf[2]<<40) | ((unsigned long long int)buf[3]<<32) | ((unsigned long long int)buf[4]<<24) | ((unsigned long long int)buf[5]<<16) | ((unsigned long long int)buf[6]<<8) | buf[7];
}

uint64_t pack754(long double f, unsigned bits, unsigned expbits)
{
	long double fnorm;
	int shift;
	long long sign, exp, significand;
	unsigned significandbits = bits - expbits - 1;

	if(f == 0.0) return 0;
	
	if(f < 0)
	{
		sign = 1;
		fnorm = -f;
	}
	else
	{
		sign = 0;
		fnorm = f;
	}

	shift = 0;
	while(fnorm >= 2.0) { fnorm /= 2.0; shift++; }
	while(fnorm < 1.0) { fnorm *= 2.0; shift--; }

	fnorm = fnorm - 1.0;
	
	significand = fnorm * ((1LL << significandbits) + 0.5f);
	exp = shift + ((1 << (expbits - 1)) - 1);

	return (sign << (bits - 1)) | (exp << (bits - expbits - 1)) | significand;
}

long double unpack754(uint64_t i, unsigned bits, unsigned expbits)
{
	long double result;
	long long shift;
	unsigned bias;
	unsigned significandbits = bits - expbits - 1;

	if(i == 0) return 0.0;

	result = (i & ((1LL << significandbits) - 1));
	result /= (1LL << significandbits);
	result += 1.0f;

	bias = (1 << (expbits - 1)) - 1;
	shift = ((i >> significandbits) & ((1LL << expbits) - 1)) - bias;
	while(shift > 0) { result *= 2.0; shift--; }
	while(shift < 0) { result /= 2.0; shift++; }

	result *= (i >> (bits - 1)) & 1 ? -1.0 : 1.0;

	return result;
}


unsigned int pack(unsigned char* buf, char* format, ...)
{
	va_list ap;

	signed char c;
	unsigned char C;

	int h;
	unsigned int H;

	long int l;
	unsigned long int L;

	long long int q;
	unsigned long long int Q;

	float f;
	double d;
	long double g;
	unsigned long long int fhold;

	char* s;
	unsigned int len;

	vec3s v;
	vec4s V;

	mat4s m;

	Vertex* vertex_data;

	unsigned int size = 0;

	va_start(ap, format);

	for(; *format != '\0'; format++)
	{
		switch(*format)
		{
			case 'c':
			{
				size += 1;
				c = (signed char)va_arg(ap, int);
				*buf++ = c;
				break;
			}
			case 'C':
			{
				size += 1;
				C = (unsigned char)va_arg(ap, unsigned int);
				*buf++ = C;
				break;
			}
			case 'h':
			{
				size += 2;
				h = va_arg(ap, int);
				packi16(buf, h);
				buf += 2;
				break;
			}
			case 'H':
			{
				size += 2;
				H = va_arg(ap, unsigned int);
				packi16(buf, H);
				buf += 2;
				break;
			}
			case 'l':
			{
				size += 4;
				l = va_arg(ap, long int);
				packi32(buf, l);
				buf += 4;
				break;
			}
			case 'L':
			{
				size += 4;
				L = va_arg(ap, unsigned long int);
				packi32(buf, L);
				buf += 4;
				break;
			}
			case 'q':
			{
				size += 8;
				q = va_arg(ap, long long int);
				packi64(buf, q);
				buf += 8;
				break;
			}
			case 'Q':
			{
				size += 8;
				Q = va_arg(ap, unsigned long long int);
				packi64(buf, Q);
				buf += 8;
				break;
			}
			case 'f':
			{
				size += 2;
				f = (float)va_arg(ap, double);
				fhold = pack754_16(f);
				packi16(buf, fhold);
				buf += 2;
				break;
			}
			case 'd':
			{
				size += 4;
				d = va_arg(ap, double);
				fhold = pack754_32(d);
				packi32(buf, fhold);
				buf += 4;
				break;

			}
			case 'g':
			{
				size += 8;
				g = va_arg(ap, long double);
				fhold = pack754_64(g);
				packi64(buf, fhold);
				buf += 8;
				break;
			}
			case 's':
			{
				s = va_arg(ap, char*);
				len = strlen(s);
				size += len + 2;
				packi16(buf, len);
				buf += 2;
				memcpy(buf, s, len);
				buf += len;
				break;
			}
			case 'v':
			{
				size += 6;
				v = va_arg(ap, vec3s);
				packvec3s(buf, v);
				buf += 6;
				break;
			}
			case 'V':
			{
				size += 8;
				V = va_arg(ap, vec4s);
				packvec4s(buf, V);
				buf += 8;
				break;
			}
			case 'm':
			{
				size += 32;
				m = va_arg(ap, mat4s);
				packmat4s(buf, m);
				buf += 32;
				break;
			}
			case '#':
			{
	 			vertex_data = va_arg(ap, Vertex*);
				int verts_len = va_arg(ap, int);

				for(int i = 0; i < verts_len; i++)
				{
					size += 18;

					packvec3s(buf, vertex_data[i].position);
					buf += 6;
					packvec4s(buf, vertex_data[i].color);
					buf += 8;
					packvec2s(buf, vertex_data[i].tex_coord);
					buf += 4;
				}
				break;
			}
		}
	}

	va_end(ap);

	return size;
}

void unpack(unsigned char* buf, char* format, ...)
{
	va_list ap;

	signed char* c;
	unsigned char* C;

	int* h;
	unsigned int* H;

	long int* l;
	unsigned long int* L;

	long long int* q;
	unsigned long long int* Q;

	float* f;
	double* d;
	long double* g;
	unsigned long long int fhold;

	char* s;
	unsigned int len, maxstrlen=0, count;

	vec3s* v;
	vec4s* V;

	mat4s* m;

	Vertex* vertex_data;

	va_start(ap, format);

	for(; *format != '\0'; format++)
	{
		switch (*format)
		{
			case 'c':
			{
				c = va_arg(ap, signed char*);
				if(*buf <= 0x7f)
				{
					*c = *buf;
				}
				else
				{
					*c = -1 - (unsigned char)(0xffu - *buf);
				}
				buf++;
				break;
			}
			case 'C':
				C = va_arg(ap, unsigned char*);
				*c = *buf++;
				break;
			case 'h':
				h = va_arg(ap, int*);
				*h = unpacki16(buf);
				buf += 2;
				break;
			case 'H':
				H = va_arg(ap, unsigned int*);
				*H = unpacku16(buf);
				buf += 2;
				break;
			case 'l':
				l = va_arg(ap, long int*);
				*l = unpacki32(buf);
				buf += 4;
				break;
			case 'L':
				L = va_arg(ap, unsigned long int*);
				*L = unpacku32(buf);
				buf += 4;
				break;
			case 'q':
				q = va_arg(ap, long long int*);
				*q = unpacki64(buf);
				buf += 8;
				break;
			case 'Q':
				Q = va_arg(ap, unsigned long long int*);
				*Q = unpacku64(buf);
				buf += 8;
				break;
			case 'f':
				f = va_arg(ap, float*);
				fhold = unpacku16(buf);
				*f = unpack754_16(fhold);
				buf += 2;
				break;
			case 'd':
				d = va_arg(ap, double*);
				fhold = unpacku32(buf);
				*d = unpack754_32(fhold);
				buf += 4;
				break;
			case 'g':
				g = va_arg(ap, long double*);
				fhold = unpacku64(buf);
				*g = unpack754_64(fhold);
				buf += 8;
				break;
			case 's':
				s = va_arg(ap, char*);
				len = unpacku16(buf);
				buf += 2;
				if(maxstrlen > 0 && len >= maxstrlen) count = maxstrlen - 1;
				else count = len;
				memcpy(s, buf, count);
				s[count] = '\0';
				buf += len;
				break;
			case 'v':
				v = va_arg(ap, vec3s*);
				*v = unpackvec3s(buf);
				buf += 6;
				break;
			case 'V':
				V = va_arg(ap, vec4s*);
				*V = unpackvec4s(buf);
				buf += 8;
				break;
			case 'm':
				m = va_arg(ap, mat4s*);
				*m = unpackmat4s(buf);
				buf += 32;
				break;
			case '#':
			{
	 			vertex_data = va_arg(ap, Vertex*);

				int verts_len = va_arg(ap, int);

				for(int i = 0; i < verts_len; i++)
				{
					vec3s position = unpackvec3s(buf);
					buf += 6;
					vec4s color = unpackvec4s(buf);
					buf += 8;
					vec2s tex_coord = unpackvec2s(buf);
					buf += 4;

					(vertex_data)[i] = (Vertex){position, color, tex_coord};
				}

				break;
			}
			default:
				if(isdigit(*format))
				{
					maxstrlen = maxstrlen * 10 + (*format - '0');
				}
				if(!isdigit(*format)) maxstrlen = 0;
		}
	}

	va_end(ap);
}
