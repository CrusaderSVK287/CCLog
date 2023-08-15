#include "json.h"
#include "options.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define JSON_BUFF_SIZE 1000000
#define BUFF_SIZE 1000

static char *json_buffer = NULL;
static int json_buffer_lenght = 0;

static int indent_level = 0;

static bool in_array = false;

const char *json_get_buffer() 
{
        cclog_debug("JSON: requested buffer");
        return json_buffer;
}

int json_init_buffer()
{
        cclog_debug("JSON: init buffer");
        json_buffer = calloc(JSON_BUFF_SIZE, sizeof(char));
        return (json_buffer) ? 0 : -1;
}

int json_free_buffer()
{
        cclog_debug("JSON: free buffer");
        if (!json_buffer)
                return -1;

        free(json_buffer);
        json_buffer = NULL;

        return 0;
}

static void write_buffer_indent(const char *string)
{
        char buff[BUFF_SIZE];
        memset(buff, 0, BUFF_SIZE);

        int ret = snprintf(buff, BUFF_SIZE, "%*s%s", indent_level * 2, "", string);

        cclog_debug("JSON: Appending \"%s\" to buffer", buff);
        strncat(json_buffer, buff, JSON_BUFF_SIZE - json_buffer_lenght);
        json_buffer_lenght += ret;
}

static void buffer_remove_last_coma()
{
        char c = json_buffer[json_buffer_lenght-2];

        cclog_debug("JSON: json_buffer_lenght-2 char: %c", c);
        if (c == ',') {
                json_buffer[json_buffer_lenght-2] = '\n';
                json_buffer[json_buffer_lenght-1] = 0;
                json_buffer_lenght--;
        }
}

int json_start_buffer()
{
        cclog_debug("JSON: Buffer start");
        if (strlen(json_buffer) != 0) {
                cclog_error("JSON: Called start buffer but buffer contains data");
                return -1;
        }

        write_buffer_indent("{\n");
        indent_level++;

        return 0;
}

int json_add_object(const char *name)
{
        cclog_debug("JSON: adding object %s", name);
        if (!name)
                return -1;

        char buff[BUFF_SIZE];

        if (!in_array) {
                snprintf(buff, BUFF_SIZE, "\"%s\": {\n", name);
        } else {
                snprintf(buff, BUFF_SIZE, "{\n");
        }

        write_buffer_indent(buff);
        indent_level++;

        return 0;
}

int json_add_array(const char *name)
{
        cclog_debug("JSON: adding array %s", name);
        if (!name)
                return -1;

        char buff[BUFF_SIZE];

        snprintf(buff, BUFF_SIZE, "\"%s\": [\n", name);
        in_array = true;

        write_buffer_indent(buff);
        indent_level++;

        return 0;
}

int json_add_parameter(const char *key, const char *value)
{
        cclog_debug("JSON: adding parameter key: %s ,value: %s", key, value);
        if (!key || !value)
                return -1;

        char buff[BUFF_SIZE];
        snprintf(buff, BUFF_SIZE, "\"%s\": %s,\n", key, value);

        write_buffer_indent(buff);
        return 0;
}

int json_end_object()
{
        cclog_debug("JSON: end object");
        buffer_remove_last_coma();

        indent_level--;
        write_buffer_indent("},\n");

        return 0;
}

int json_end_array()
{
        cclog_debug("JSON: end array");
        buffer_remove_last_coma();

        indent_level--;
        in_array = false;
        write_buffer_indent("],\n");

        return 0;
}

int json_end_buffer()
{
        cclog_debug("JSON: end buffer");
        buffer_remove_last_coma();

        indent_level--;
        write_buffer_indent("}\n");

        return (indent_level == 0)? 0 : -1;
}

