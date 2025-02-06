# CCLog
### Introduction
CCLog ,Customizable C Logger, is a logging library written in C for Linux based operating systems.\
It is simple to use and provides customizability features. Now also includes a C++ wrapper (Read bottom of readme)
***
## Main Features
CCLog allows you to:
* Specify your logging type. Eighter log everything to a single file or create a new file with each initialisation
* Multiple rulesets (log_levels) telling how and where a log message is to be outputed
* Create custome rulesets. Specifiy color and format of the message
* Specify a callback function for a ruleset
* Customize how the full log message looks
* Export or import a logger configuration with json file
* Host a http server to access the configuration or log remotely
***
## How to compile
To compile the library, follow these steps
1. Clone this repository
2. Run `make`
3. (Optitional) Run `make install`

When compiling a binary with the library used, don't forget to link it with `-lcclog`. (Only works if you ran `make install`)

Default make target will compile the sources and generate a .so file in ./bin/libcclog.so.\
The optitional install target will copy the result binary and the header file to the coresponding places, you will than be able to link the library dynamicaly.
***
## Basic usage

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
## Convenience macros
There are several macros provided for convenience
* if_failed(val, label) - if val is other than 0, jump to the label
* if_null(val, label) - if val is null, jump to the label
* if_failed_log(val, label, log_level, priv, msg, ...) - if val is other than 0, make a log and jump to the label
* if_null_log(val, label, log_level, priv, msg, ...) - if val is null, make a log and jump to the label

To use these macros you need to include this header file
```c
#include <cclog_macros.h>
```

if_failed macros have a positive and negative variants which fire if the val is eighter positive or negative but not zero.\
These variants have eighter p or n at the end. Example: `if_failed_log_p` will log and jump to label if value is greater than 0.
There are way more macros than this. Look inside documentation or src/cclog_macros.h file
***
## Example
Example for a simple program using cclog library
```c
// File: logger.c

#include <stdio.h>
#include <cclog.h>
#include <cclog_macros.h>

int main()
{
    /* 
     * Initialise the logger.
     * We will be using a single file called my_log_file
     * We dont need to specify proc_name since we are sure
     * path isnt NULL
     */
    cclogger_init(LOGGING_SINGLE_FILE, "./my_log_file", NULL);
	
    /* We write a message to the file only */
    cclog(CCLOG_LEVEL_MSG, NULL, "Lets do some logging");
	
    /* We write a message both to file and to terminal */
    int val = -1;
    if_failed_log(val, error, CCLOG_LEVEL_ERR, NULL, "Error, bad result");

    printf("This should not appear\n");

error:
    /* Uninitialise logger */
    cclogger_uninit();

    return 0;
}
```
my_log_file.log:
```
[2023-8-23_16:9:52]logger.c:10:Lets do some logging
[2023-8-23_16:9:52]logger.c:13:Error, bad result
```
stderr output:
```
[2023-8-23_16:9:52]logger.c:13:Error, bad result
```

## C++ Wrapper
A C++ wrapper was provided for better integration with C++ applications.
You can access all the functions through `cclog` namespace.
Some features are limited, such as callbacks, *which are not working* at the moment.
There is a warning for each function if there is an issue with it.
Use he C++ wrapper by including `cclog.hpp` instead of `cclog.h`.
