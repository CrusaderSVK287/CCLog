#ifndef __CCLOGER_JSON_H__
#define __CCLOGER_JSON_H__

const char *json_get_buffer();

/**
 * Function initialises json buffer into which the json string will be built
 * The buffer is cleared of all previous data if any are present
 * @return 0 on success, -1 on failure 
 */
int json_init_buffer();

/**
 * Erases and frees json buffer from memory. This function is not neccessary
 * to be called, since the buffer is allocated only once, but it should in case
 * the memory is needed elsewhere
 * @return 0 on success, -1 on failure (cannot free buffer)
 */
int json_free_buffer();

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
int json_start_buffer();

/* Function adds object to json like this: "name": { */
int json_add_object(const char *name);

/* Function adds array to json like this: "name": [ */
int json_add_array(const char *name);

/* 
 * Function adds parameter to json like this: "key": value 
 * NOTE: if value is string, be sure to specify double quote in value.
 * e.g. json_add_parameter("key_name", "\"key_value\"")
 * Otherwise the value might be invalid
 */
int json_add_parameter(const char *key, const char *value);

/* Function ends object by inserting } */
int json_end_object();

/* Function ends an array by inserting ] */
int json_end_array();

/* 
 * Function ends the buffer with the last } and checks whether the buffer is 
 * ended correctly (no unclosed arrays or objects)
 */
int json_end_buffer();

#endif // !__CCLOGER_JSON_H__

