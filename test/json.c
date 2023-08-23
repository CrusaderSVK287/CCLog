#include "tests.h"
#include "../src/json.h"
#include "../src/cclog.h"
#include <stdio.h>
#include <stdlib.h>

static void dump_json_param(const json_param_t *param)
{
        if (!param) {
                printf("Param is NULL\n");
                return;
        }

        switch (param->type) {
                case JSON_PARAM_NUMBER: printf("Number: %d\n", param->number); break;
                case JSON_PARAM_BOOELAN: printf("Boolean: %s\n", (param->boolean)?"true":"false"); break;
                case JSON_PARAM_STRING: printf("String: %s\n", param->string); break;
                default:
                        break;
        }
}

void json()
{
        const json_param_t *param = NULL;
        const char *json = "{\n"
                "  \"variable\": 8348,\n"
                "  \"another\": \"another_value\",\n"
                "  \"test_var\": true\n"
                "}\n";

        const char *json_object = "{\n"
                "  \"variable\": 8348,\n"
                "  \"another\": \"another_value\",\n"
                "  \"nested_object\": {\n"
                "     \"nested_var\": \"nested_{}string\",\n"
                "     \"nested_num\": 56\n"
                "     \"nested_nested_object\": {\n"
                "          \"nested_nested_number\": 42\n"
                "     }\n"
                "   },\n"
                "  \"nested\": {\n"
                "     \"nested_bool\": false\n"
                "   },\n"
                "  \"test_var\": true\n"
                "}\n";
        const char *json_array = "{\n"
                "  \"variable\": 8348,\n"
                "  \"another\": \"another_value\",\n"
                "  \"nested_array\": [\n"
                "      {\n"
                "        \"nested_var\": \"nested_string\",\n"
                "        \"nested_num\": 1\n"
                "      },\n"
                "      {\n"
                "        \"nested_var\": \"nested_string2\",\n"
                "        \"nested_num\": 2\n"
                "      },\n"
                "      {\n"
                "        \"nested_var\": \"nested_string3\",\n"
                "        \"nested_num\": 3\n"
                "      },\n"
                "      {\n"
                "        \"nested_object\": {\n"
                "             \"nested_nested_var\": true\n"
                "         }\n"
                "      }\n"
                "  ],\n"
                "  \"test_var\": true\n"
                "}\n";

        printf("%s\n\n\n%s\n", json_object, json_array);

        param = json_get_param(json, "variable");
        dump_json_param(param);
        param = json_get_param(json, "another");
        dump_json_param(param);
        param = json_get_param(json, "test_var");
        dump_json_param(param);

        char *object = json_get_object(json_object, "nested_object");
        if (!object)
                printf("Failed\n");
        else {
                printf("Object: VVV\n%s\n", object);
                param = json_get_param(object, "nested_var");
                dump_json_param(param);
                param = json_get_param(object, "nested_num");
                dump_json_param(param);

                free(object);
        }

        char *array = json_get_array(json_array, "nested_array");
        const char *array_object = NULL;
        if (!array)
                printf("array failed\n");
        else {
                printf("Array: VVV\n%s\n", array);

                array_object = json_get_object_from_array(array, 0);
                printf("Object at index 0: VVV\n%s\n", array_object);
                array_object = json_get_object_from_array(array, 1);
                param = json_get_param(array_object, "nested_num");
                dump_json_param(param);
                printf("Object at index 1: VVV\n%s\n", array_object);
                array_object = json_get_object_from_array(array, 2);
                printf("Object at index 2: VVV\n%s\n", array_object);
                array_object = json_get_object_from_array(array, 3);
                printf("Object at index 3: VVV\n%s\n", array_object);
                array_object = json_get_object_from_array(array, 8);
                printf("Object at index 8: VVV\n%s\n", array_object);
                free(array);
        }
}

