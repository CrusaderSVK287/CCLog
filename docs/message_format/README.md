# Message format

CClogger allows you to customize how the log messages will look like. You can set the default log message format using this function. 
```c
int cclogger_set_default_message_format(const char *str);
```
This format will be used by every log level, unless overriden.
The function takes only one argument, the string that dictates how the messages will look.
The syntax of this string is extremely simple. Here is an example:\
`(${TIME}) ${FILE} : ${MSG}`\
As you can see, the string looks similiari to what you might find in a bash script. All characters will be copied to the result message, while variables inside ${} blocks will be replaced.

## Example
Lets work with the format string provided earlier. Before logging, we set the default message format
```c
cclogger_set_default_message_format("(${TIME}) ${FILE} : ${MSG}");
```
Now we can make a log, and see the result:
```
cclog(CCLOG_LEVEL_INFO, NULL, "This is my message");
```
In the stderr we get the following output: `(00:09:23) message_format.c : This is my message`

## Variables
This readme wont be updated if new variables are added, you can see the full list in the function description inside the header file. As of time of writing, this is the list of variables that are build into the library
* MSG - The message specified by user
* FILE - File in which the log was called
* LINE - Line of code on which the log was called
* FUNCTION - Name of function in which the log was called
* DATE - Current date in YYYY-MM-DD format
* TIME - Current time in HH:MM:SS format
* PID - pid of the running process
* ERRNO - error number as a digit
* ERMSG - error message indicated by errno, got from strerror()
* YYYY - Current year
* MM - Current month (1 to 12)
* DD - Current day (1 to 31)
* HOUR - Current hour
* MIN - Current minute
* SEC - Current second

If the variable parsed from the string is not one of the inbuild ones, the logger will try to get it as an environmental variable, if this fails as well, the variable will simply not be used

