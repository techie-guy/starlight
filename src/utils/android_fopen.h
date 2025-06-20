#pragma once

#if defined(_PLATFORM_ANDROID)

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

FILE* android_fopen(const char* filename, const char* mode);

#define fopen(name, mode) android_fopen(name, mode)

#ifdef __cplusplus
}
#endif


#endif
