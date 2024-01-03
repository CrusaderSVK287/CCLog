#include "json.h"
#include "options.h"
#include "utils/defines.h"
#include "utils/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/mman.h>

#define JSON_BUFF_SIZE 10000
#define BUFF_SIZE 1000

bool cclog_json_is_valid(const char *json) {
        int len = strlen(json);
        int braces = 0, brackets = 0;
        bool inString = false;

        for (int i = 0; i < len; i++) {
                char c = json[i];

                if (c == '{') {
                        if (!inString)
                                braces++;
                } else if (c == '}') {
                        if (!inString)
                                braces--;
                        if (braces < 0)
                                return false;
                } else if (c == '[') {
                        if (!inString)
                                brackets++;
                } else if (c == ']') {
                        if (!inString)
                                brackets--;
                        if (brackets < 0)
                                return false;
                } else if (c == '"') {
                        inString = !inString;
                }
        }

    return (braces == 0 && brackets == 0);
}

/* actual buffer and its size */
static char *json_buffer = NULL;
static int json_buffer_lenght = 0;

/* Variable tracks current indent level during building to correctly prepend 
 * whitespaces */
static int indent_level = 0;

/* Variable tracks whether we are currently in and array while building a 
 * string in buffer */
static bool in_array = false;

/* clears buffer */
void cclog_json_buffer_clear()
{
        memset(json_buffer, 0, JSON_BUFF_SIZE);
        json_buffer_lenght = 0;
        indent_level = 0;
}

const char *cclog_json_get_buffer() 
{
        cclog_debug("JSON: requested buffer");
        return json_buffer;
}

int cclog_json_init_buffer()
{
        cclog_debug("JSON: init buffer");
        json_buffer = mmap(NULL, JSON_BUFF_SIZE, PROT_READ | PROT_WRITE, 
                           MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        return (json_buffer) ? 0 : -1;
}

/* Writes string to buffer with prepended whitespaces based on indent level */
static void write_buffer_indent(const char *string)
{
        char buff[BUFF_SIZE];
        memset(buff, 0, BUFF_SIZE);

        int ret = snprintf(buff, BUFF_SIZE, "%*s%s", indent_level * 2, "", string);

        cclog_debug("JSON: Appending \"%s\" to buffer", buff);
        strncat(json_buffer, buff, JSON_BUFF_SIZE - json_buffer_lenght);
        json_buffer_lenght += ret;
}

/* Since everything is inserted with a coma at the end of line, when we are */
/* ending an object or array, we need to remove the coma from the last line */
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

int cclog_json_start_buffer()
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

int cclog_json_add_object(const char *name)
{
        cclog_debug("JSON: adding object %s", name);
        if (!name)
                return -1;

        char buff[BUFF_SIZE];

        /* If we are in an array, dont add a key */
        if (!in_array) {
                snprintf(buff, BUFF_SIZE, "\"%s\": {\n", name);
        } else {
                snprintf(buff, BUFF_SIZE, "{\n");
        }

        write_buffer_indent(buff);
        indent_level++;

        return 0;
}

int cclog_json_add_array(const char *name)
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

int cclog_json_add_parameter(const char *key, const char *value)
{
        cclog_debug("JSON: adding parameter key: %s ,value: %s", key, value);
        if (!key || !value)
                return -1;

        char buff[BUFF_SIZE];
        snprintf(buff, BUFF_SIZE, "\"%s\": %s,\n", key, value);

        write_buffer_indent(buff);
        return 0;
}

int cclog_json_end_object()
{
        cclog_debug("JSON: end object");
        /* Remove , from the last parameter in the object */
        buffer_remove_last_coma();

        indent_level--;
        write_buffer_indent("},\n");

        return 0;
}

int cclog_json_end_array()
{
        cclog_debug("JSON: end array");
        /* Remove last , in the last object in the array */
        buffer_remove_last_coma();

        indent_level--;
        in_array = false;
        write_buffer_indent("],\n");

        return 0;
}

int cclog_json_end_buffer()
{
        cclog_debug("JSON: end buffer");
        /* Remove last , in paramter, object or array in the entire buffer */
        buffer_remove_last_coma();

        indent_level--;
        write_buffer_indent("}\n");

        /* Simple check whether everything was closed */
        return (indent_level == 0)? 0 : -1;
}


/**
 * ############################################################################
 *         This section contains functions used to read a json file
 * ############################################################################
 */

static char *json_file_buffer = NULL;

const char *cclog_json_get_file_buffer()
{
        cclog_debug("Requested json file buffer");
        return json_file_buffer;
}

void cclog_json_free_file_buffer()
{
        if (json_file_buffer) {
                free(json_file_buffer);
                json_file_buffer = NULL;
        }
}

int cclog_json_read_file(const char *path)
{
        if (!path)
                return -1;

        int rv = -1;

        cclog_debug("JSON: Opening file %s", path);
        int fd = open(path, O_RDONLY);
        if (fd < 0) {
                cclog_error("JSON: Could not open file %s", path);
                goto error;
        }

        /* Clear buffer if it exists, initialise it otherwise */
        if (json_file_buffer) {
                memset(json_file_buffer, 0, JSON_BUFF_SIZE);
        } else {
                json_file_buffer = calloc(1, JSON_BUFF_SIZE);
                if (!json_file_buffer) {
                        cclog_error("Could not allocate space for json file buffer");
                        goto error;
                }
        }

        if (read(fd, json_file_buffer, JSON_BUFF_SIZE) < 0) {
                cclog_error("JSON: Error reading file %s", path);
                rv = -1;
        }

        close(fd);
        cclog_debug("Json buffer contents:\n%s\n", json_buffer);

        rv = 0;
error:
        return rv;
}

cclog_json_param_t *cclog_json_get_param(const char *json, const char *key)
{
        if (!json || !key)
                return NULL;

        cclog_debug("JSON: aquiring value for key %s", key);

        static cclog_json_param_t rv = {};

        char format_buff[BUFSIZ];
        static char scan_buff[BUFSIZ];
        memset(scan_buff, 0, BUFSIZ);

        /* Format the string to look like this "name_of_param": 
         * and find this substring in the main string */
        sprintf(format_buff, "\"%s\": ", key);
        const char *offset = strstr(json, format_buff);
        if (!offset) {
                cclog_debug("JSON: Could not find key %s", key);
                return NULL;
        }

        /* Add this to the format string to parse the value */
        strcat(format_buff, "%[^\n]");
        int ret = sscanf(offset, format_buff, scan_buff);

        if (scan_buff[strlen(scan_buff)-1] == ',')
                scan_buff[strlen(scan_buff)-1] = 0;

        if (ret != 1) {
                cclog_error("JSON: Could not find value for key variable");
                return NULL;
        }

        /* If the value is true or false its a boolean*/
        if (!strcmp(scan_buff, "true") || !strcmp(scan_buff, "false")) {
                rv.type = JSON_PARAM_BOOELAN;
                rv.boolean = cclog_str_to_bool(scan_buff);
                cclog_debug("JSON: key %s is a boolean of value %d", key, rv.boolean);
                goto exit;
        }

        /* If the value is a number, it is parsed here */
        int tmp = 0;
        if (sscanf(scan_buff, "%d", &tmp) == 1) {
                rv.type = JSON_PARAM_NUMBER;
                rv.number = tmp;
                cclog_debug("JSON: key %s is integer with value %d", key, rv.number);
                goto exit;
        }
        
        /* Otherwise its a string */
        static char string_buff[BUFSIZ];
        if (sscanf(scan_buff, "\"%[^\"]", string_buff) == 1) {
                rv.type = JSON_PARAM_STRING;
                rv.string = string_buff;
                cclog_debug("JSON: key %s is a string with value %s", key, rv.string);
                goto exit;
        }

exit:
        return &rv;
}

static char *json_get_nested_structure(const char *json, const char *key, 
                                             const char *start_symbol, 
                                             const char *end_symbol) 
{
        if (!json || !key || !start_symbol || !end_symbol)
                return NULL;

        /* Allocate buffer. This buffer is not static to allow for more nested 
         * structures to be retrieved without loosing reference to the parent ones 
         */
        int buff_i = 0;
        char *buff = calloc(1, BUFSIZ);
        if (!buff)
                return NULL;

        /* Format a string to find the structure based on starting symbol. 
         * [ for array or  { for object 
         */
        char format_buff[BUFSIZ];
        int ret = sprintf(format_buff, "\"%s\": %s", key, start_symbol);
        char *offset = strstr(json, format_buff);
        if (!offset) {
                free(buff);
                return NULL;
        }
        
        /* This is to skip the "structure_name": 
         * part BUT keep the starting symbol 
         */
        offset += ret - 1;

        /* in string keeps track whether we are currently inside a string 
         * nested_level keeps track of how many objects/structures we need to 
         * escape to be at the end of the one we want
         * It starts at -1 because the first start symbol will immediately 
         * increase it to 0 
         */
        bool in_string = false;
        int nested_level = -1;
        
        /* Will copy every character from start of the object to the end of it */
        while ((*offset != *end_symbol || in_string || nested_level > 0) && *offset != 0) {
                buff[buff_i++] = *offset;

                if (*offset == *start_symbol && !in_string)
                        nested_level++;
                if (*offset == *end_symbol && !in_string)
                        nested_level--;
                if (*offset == '"')
                        in_string = !in_string;

                if (buff_i >= BUFSIZ - 2) {
                        break;
                }

                offset++;
        }

        /* Add the missing end symbol and return */
        strcat(buff, end_symbol);

        return buff;
}

char *cclog_json_get_object(const char *json, const char *key)
{
        return json_get_nested_structure(json, key, "{", "}");
}

char *cclog_json_get_array(const char *json, const char *key)
{
        return json_get_nested_structure(json, key, "[", "]");
}

const char *cclog_json_get_object_from_array(char *array, int index)
{
        if (!array || index < 0)
                return NULL;

        static char buff[BUFSIZ];
        memset(buff, 0, BUFSIZ);
        
        bool in_string = false;
        int nested_level = 0;
        int current_index = 0;
        int buff_i = 0;

        /* Skip X amount of objects to get to the one we want 
         * These loops are basically the same as the one at json_get_nested_structure(). 
         * For more inforamtion, please refer to comments there 
         */
        while (current_index < index && *array != 0) {
                if (*array == '{' && !in_string)
                        nested_level++;
                if (*array == '}' && !in_string) {
                        nested_level--;
                        if (nested_level == 0) {
                                current_index++;
                        }
                }
                if (*array == '"')
                        in_string = !in_string;

                array++;
        }

        cclog_debug("JSON: Current array index: %d", current_index);
        /* 
         * In case the index specified is too large, e.g. there is not object 
         * with such index, return null 
         */
        if (*array == 0) {
                return NULL;
        } 

        /* Skip any whitespace characters */
        while(isspace(*(++array)));

        nested_level = 0;
        while ((*array != ']' || in_string || nested_level > 0) && *array != 0) {
                buff[buff_i++] = *array;

                if (*array == '{' && !in_string)
                        nested_level++;
                if (*array == '}' && !in_string) {
                        cclog_debug(STR_YEL "decreasing level from %d" STR_RESET "\n", nested_level);
                        nested_level--;
                        if (nested_level == 0) {
                                break;
                        }
                }
                if (*array == '"')
                        in_string = !in_string;

                if (buff_i >= BUFSIZ - 2) {
                        break;
                }

                array++;
        }

        return buff;
}

