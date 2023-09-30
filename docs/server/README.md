# Server

The server allows you to remotely access the configuration and the log file contents

## Starting the server

To start the server, you need to eighter load it from config, from a paramter like this
```json
"server": {
  "port": port_num
}
```
Or use this function.
```c
int cclogger_server_start(int port);
```
This will create a server process that will be hosted on port specified in the argument. This function returns the server pid if it was successfully created, but you can retreive the pid also using
```c
int cclogger_server_pid();
```
## Stopping the server

To stop the server all you need to do is call this function:
```c
int cclogger_server_stop();
```
**Note**: This function should be called whether you started the server yourself or you started it when loading configuration

## Example
You can run an example server by executing `make` and then `./server`. It will run for one minute or until you send interrupt signal to it (ctrl + c). During its uptime you can visit the page on http://localhost:8080

