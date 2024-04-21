#pragma once

#include <cJSON.h>
#include <stdbool.h>

cJSON* json_parse(const char* json_data_string);

char* json_print(cJSON* root);

cJSON* json_create_object();
cJSON* json_create_array();
cJSON* json_create_float_array(float* data, int count);
void json_delete_object(cJSON* cjson);

bool json_add_item_to_object(cJSON* root, char* string, cJSON* item);
bool json_add_item_to_array(cJSON* array, cJSON* item);


cJSON* json_get_object(cJSON* cjson, const char* string);
double json_get_double(cJSON* cjson, const char* string);
int json_get_int(cJSON* cjson, const char* string);
char* json_get_string(cJSON* cjson, const char* string);
cJSON* json_get_array_item(cJSON* array, int index);
