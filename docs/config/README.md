# Configuration

### Prerequisites
It is recommended to first read and understand chapters about log levels and default messages to fully understand how configuration works.

## How to export configuration
Exporting the current cclogger configuration is as simple as calling this function.
```c
int cclogger_export_config_json(const char *path);
```
This function takes one argument, path to the json file to which the configuration will be saved. Here is an example of how a default configuration looks like:
```json
{
  "log_file_path": "./my_log_file.log",
  "log_method": 0,
  "def_msg_format": "[${DATE}_${TIME}]${FILE}:${LINE}:${MSG}",
  "log_levels": [
    {
      "log_to_tty": true,
      "log_to_file": false,
      "color": 0
    },
    {
      "log_to_tty": false,
      "log_to_file": true,
      "color": 0
    },
    {
      "log_to_tty": true,
      "log_to_file": true,
      "color": 2
    }
  ]
}
```
Appart from log_file_path and log_method, this is how a default configuration looks like.

## How to load configuration
Similiary to exporting a configuration, you can import one from a json file using this function:
```c
int cclogger_load_config_json(const char *path, cclog_callback_mapping_t cb_mappings[]);
```
The function takes two arguments, path to the json file from which the config should be loaded and an array of logger callbacks. To remind how this array is supposed to be declared:
```c
static cclog_callback_mapping_t cclog_cb_maps[] = {
        {"log_callback", log_callback},
        {NULL, NULL}
};
```
If you are sure that no callbacks are used, this parameter can be NULL.

## Example
You can run an example program using `make` and then `./config`. And example .json file is prepared that will be used as a config.

