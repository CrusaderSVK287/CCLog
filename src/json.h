#ifndef __CCLOGER_JSON_H__
#define __CCLOGER_JSON_H__

#include <stdbool.h>

/* Enum indicating type of json parameter expected when fetching */
typedef enum json_param_type {
    JSON_PARAM_STRING,
    JSON_PARAM_NUMBER,
    JSON_PARAM_BOOELAN,
} cclog_json_param_type_t;

typedef struct json_param {
    cclog_json_param_type_t type;
    union {
        int number;
        bool boolean;
        char *string;
    };
} cclog_json_param_t;

/* Function returns boolean indicating whether json string is valid or not */
bool cclog_json_is_valid(const char *json);

/* Function returns the pointer to the json buffer */
const char *cclog_json_get_buffer();

/* Function returns json buffer used to read from file */
const char *cclog_json_get_file_buffer();

/* Frees json file buffer, needs to be used after calling cclog_json_read_file */
void cclog_json_free_file_buffer();

/**
 * Function initialises json buffer into which the json string will be built
 * The buffer is cleared of all previous data if any are present
 * @return 0 on success, -1 on failure 
 */
int cclog_json_init_buffer();

/**
 * Erases contents of json buffer. Needs to be called before new buffer is 
 * initialised using cclog_json_start_buffer
 */
void cclog_json_buffer_clear();

/**
 * Following functions all do the same thing: they manipulate the buffer itself.
 * They all return 0 on success and -1 on error but keep in mind that not all 
 * of these functions can actually return an error.
 * All critical errors are displayed on runtime using cclog_error() 
 */

/*
 * Function must be called before any other data is written to the buffer
 * Checks whether the buffer is empty and inserts the initial {
 */
int cclog_json_start_buffer();

/* Function adds object to json like this: "name": { */
int cclog_json_add_object(const char *name);

/* Function adds array to json like this: "name": [ */
int cclog_json_add_array(const char *name);

/* 
 * Function adds parameter to json like this: "key": value 
 * NOTE: if value is string, be sure to specify double quote in value.
 * e.g. cclog_json_add_parameter("key_name", "\"key_value\"")
 * Otherwise the value might be invalid
 */
int cclog_json_add_parameter(const char *key, const char *value);

/* Function ends object by inserting } */
int cclog_json_end_object();

/* Function ends an array by inserting ] */
int cclog_json_end_array();

/* 
 * Function ends the buffer with the last } and checks whether the buffer is 
 * ended correctly (no unclosed arrays or objects)
 */
int cclog_json_end_buffer();

/**
 * Function reads a file into a file buffer. This is not the same buffer as the 
 * one for building the string, retreive using cclog_json_get_file_buffer()
 */
int cclog_json_read_file(const char *path);

/**
 * Function retrieves a value of a key from json. This key has be a direct 
 * member of the object. While it would work on nested objects, it might behave 
 * strangely, this is still a very early code and is not meant for complex 
 * json data structures.
 * Returns a structure that contains a type of parameter returned and its 
 * value in the appropriate element
 */
cclog_json_param_t *cclog_json_get_param(const char *json, const char *key);

/**
 * Retrieves an object from json string based on key.
 * HAS TO BE FREED WHEN NOT NEEDED ANYMORE 
 */
char *cclog_json_get_object(const char *json, const char *key);

/**
 * Retrieves and array from a json string based on key. Use cclog_json_get_object_from_array 
 * to retrieve objects on specific indexes 
 * HAS TO BE FREED WHEN NOT NEEDE ANYMORE 
 */
char *cclog_json_get_array(const char *json, const char *key);

/**
 * Retrieves object from a json array based on index starting from 0 
 */
const char *cclog_json_get_object_from_array(char *json_array, int index);

#endif // !__CCLOGER_JSON_H__

