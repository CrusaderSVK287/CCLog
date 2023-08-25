# Basic usage

To start using cclog library, you need to be aware of 3 functions.
* cclogger_init()
* cclogger_uninit()
* cclog()

### cclogger_init()
Function cclogger_init() initialises the logger and must be called before any other cclogger functions.\
It opens a log file, sets up log_levels and configuration etc... more on that later. The declaration of the function looks like this:
```c
int cclogger_init(logging_type_t type, const char *path, const char *proc_name);
```
**type**\
The first argument is type of logging. this is an enum, right now there are two types of logging:
* LOGGING_SINGLE_FILE
* LOGGING_MULTIPLE_FILES

Single file logging writes all logs to the same file.\
Multiple files logging creates a new file each time the logger is initialised.\

**path**\
The path is a path to the file where the logs will be stored. Lets asume `example/path/filename` to be our path.\
If logging type is single file, the file will be created like this `example/path/filename.log`.\
If logging type is multiple files, like this `example/path/filename-[YYYY-MM-DD_HH:MM:SS].log` with the timestamp being the current local time of logger inititialisation

**proc_name**\
This is more of a failsafe in case the path is null. It acts the same way, only that the path is `/home/user/proc_name.log`.

**return**\
This function returns 0 on success and -1 on failure.

**Note:** Eighter path or proc_name MUST be specified, otherwise the initialisation fails.



### cclogger_uninit()
Function cclogger_uninit() uninitialises the logger. While it is not strictly necessary to call this function, it is highly recommended, especialy if you need to uninitialise the logger in the middle of the process rather than at the end.
It frees memory allocated by the logger, stops server if running, closes file/s etc..

### cclog()
The actuall logging function.\
Disclaimer: this is a macro, the function is \_cclogger\_log but for simplicity a macro was created. As such we will refer to this as a function.\
The declaration of this function looks like this:
```c
int cclog(log_level, priv_data, msg, ...);
```
**log_level**\
Each log has a log_level. Log levels are basically a set of rules or configurations stating how and where the log message will be presented.\
This is a simplified explanation, look for documentation in docs directory to learn more about this topic.\
There are 3 default log levels defined by this enum:
```c
typedef enum cclog_default_log_levels {
    CCLOG_LEVEL_INFO,   /* Print to stderr in default tty color only */
    CCLOG_LEVEL_MSG,    /* Print to file only */
    CCLOG_LEVEL_ERR,    /* Print to file and to stderr in red color */
} cclog_default_log_levels_t;
```
**priv_data**\
Pointer to private data that is passed into a callback function at the end of logging process, if the log_level has a callback specified.\
Again, you can read more about this in the documentation.\

**msg**\
Format string that lets you describe your log message. Use the same formating as for `printf` function.\
After this parameter, the variable format variables are specified, again, same as for `printf`.\
***
# Example 
You can run the example program bu executing `make` and than `./basic_usage`.\
The program will create two logs in a my_log_file.log file and print out one message in red to stderr
