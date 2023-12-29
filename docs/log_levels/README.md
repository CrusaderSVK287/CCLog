# Log levels

Log levels are sets of rules that dictate how a log message will look and where it will be displayed.

There are 3 default log levels:
* CCLOG_LEVEL_INFO: Print to stderr in default color
* CCLOG_LEVEL_MSG: Print to file only
* CCLOG_LEVEL_ERR: Print to file and to stderr in red color

Each log level has its own index. The default ones have indexes 0, 1 and 2 respectivelly. Your custom log levels will start with indexes from 3, incrementing by one in order in which you add them/load them from config file.

### How to add a custom log level
To add a custom log level you can use cclogger_add_log_level function which has the following declaration:
`int cclogger_add_log_level(bool log_to_file, bool log_to_tty, cclog_tty_log_color_t color, cclog_callback_mapping_t *callback, const char *msg_format_override, int verbosity);`

This function will create a log level and save it to the loggers configuration. Your created log levels will start with index of 3 (because 0 - 2 are taken by defaults) unless you erase all log levels before (more on that later). Here is the explanation for the arguments this function takes.
* bool log_to_file: whether the message should be logged to the file
* bool log_to_tty: whether the message should be printed to stderr
* cclog_tty_log_color_t: enum, indicating terminal color in which the message will be displayed (not needed if log_to_tty is false)
* cclog_callback_mapping_t \*callback: pointer to a callback function mapping that should be called at the end of logging (More on this later)
* const char \*msg_format_override: override of the default format message (see message format documentation)
* int verbosity: Set the verbosity of this log level. If the verbosity is higher than logger set verbosity, the log will not be made 

### Removing log levels
There is no way yet to remove a specifil log level with specific index.\
You can use function 
```c
void cclogger_reset_log_levels();
```
To remove all currently loaded log levels, including the default ones. Once using this function you have to add at least one custom log level. 
** Beware that this level will start with index 0 !**

### Verbosity 
Each log level has its verbosity. Verbosity is a general level at which the logger operates. Logger verbosity is set by function
```c 
void cclogger_set_verbosity_level(int verbosity);
```
logger verbosity is 10 by default and the default log levels all have verbosity of 5. Verbosity determines, whether a log will be performed 
or not. If logger verbosity is higher ot equal to the verbosity level of a log level, the log will be performed. If log level has higher verbosity than logger, log will no be made.

### Callbacks
Callback functions allow you to execute some code after the logging process. They are declared like this\
`int log_callback(const char *msg, void *priv)` (See example code).\
They take two arguments that are passed into the function by the \_cclogger\_log function.
* const char \*msg: generated log message
* void \*priv: Your private data passed into the cclog() function

Generated log message is simply the message that was logged to file or to terminal.\
priv is pointer you can pass to the cclog() macro when calling it as a second argument. As we can see in the example code
```c
int num = 30;
printf("callback return: %d\n", cclog(4, &num, "Testing callback return value, should be %d", num));
```
When we take a look at our callback function, we can see that if the private data is provided, it returns the integer pointer to by it. 
And indeed, if we run the program, in stdout we see "callback return: 30". The private data is not touched in the cclog() function itself, it is only passed to the callback.\

### Adding Callbacks
To add a callback, first you have to create a mapping of the callbacks. This is done so that the callbacks can be loaded from a config file. 
The calback map looks like this
```c
static cclog_callback_mapping_t cclog_cb_maps[] = {
        {"log_callback", log_callback},
        {"another_cb", another_cb},
        {NULL, NULL}
};
```
Its and array of cclog_callback_mapping_t structs that have two fields, name of the callback function as a string and pointer to said function.\
Note: the name of the function and the string name of the function dont have to be the same.\
At the end of this array a sentinel guard, {NULL, NULL} MUST be present, otherwise when loading config and the logger doesnt find the function map 
for whatever reason, the program will crash. To add a callback to log level you need to call cclogger_add_log_level like this 
```c
cclogger_add_log_level(false, true, CCLOG_TTY_CLR_BLU, &cclog_cb_maps[0], NULL);
```
This is if you use the same way of creating the array as shown above. It just has to be a pointer to the cclog_callback_mapping_t struct.

### Usefull functions
Take a look at functions `int cclogger_last_log_return_value();` and `int cclogger_recall_last_callback(void *priv);`\
Theese functions are usefull when you need to eighter get the return value of last log or recall the last callback with different private data

