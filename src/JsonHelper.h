#pragma once

#include <cJSON.h>

cJSON* json_parse(const char* json_data_string);
void json_delete_object(cJSON* cjson);

cJSON* json_get_object(cJSON* cjson, const char* string);
double json_get_double(cJSON* cjson, const char* string);
int json_get_int(cJSON* cjson, const char* string);
char* json_get_string(cJSON* cjson, const char* string);
