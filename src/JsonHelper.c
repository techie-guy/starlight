#include "JsonHelper.h"

cJSON* json_parse(const char* json_data_string)
{
	return cJSON_Parse(json_data_string);
}

void json_delete_object(cJSON* cjson)
{
	cJSON_Delete(cjson);
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
