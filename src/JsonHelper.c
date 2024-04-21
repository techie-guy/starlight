#include "JsonHelper.h"
#include "cJSON.h"

cJSON* json_parse(const char* json_data_string)
{
	return cJSON_Parse(json_data_string);
}

char* json_print(cJSON* root)
{
	return cJSON_Print(root);
}

cJSON* json_create_object()
{
	return cJSON_CreateObject();
}

void json_delete_object(cJSON* cjson)
{
	cJSON_Delete(cjson);
}

cJSON* json_create_array()
{
	return cJSON_CreateArray();
}

cJSON* json_create_float_array(float* data, int count)
{
	return cJSON_CreateFloatArray(data, count);
}

bool json_add_item_to_object(cJSON* root, char* string, cJSON* item)
{
	return cJSON_AddItemToObject(root, string, item);
}

bool json_add_item_to_array(cJSON* array, cJSON* item)
{
	return cJSON_AddItemToArray(array, item);
}

cJSON* json_get_object(cJSON* cjson, const char* string)
{
	return cJSON_GetObjectItemCaseSensitive(cjson, string);
}

int json_get_int(cJSON* cjson, const char* string)
{
	return cJSON_GetObjectItemCaseSensitive(cjson, string)->valueint;
}

double json_get_double(cJSON* cjson, const char* string)
{
	return cJSON_GetObjectItemCaseSensitive(cjson, string)->valuedouble;
}

char* json_get_string(cJSON* cjson, const char* string)
{
	return cJSON_GetObjectItemCaseSensitive(cjson, string)->valuestring;
}

cJSON* json_get_array_item(cJSON* array, int index)
{
	return cJSON_GetArrayItem(array, index);
}
