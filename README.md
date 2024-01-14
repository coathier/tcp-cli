# tcp-cli
A commandline TCP client written in C with ncurses, the TCP server is just here for testing.

## Dependencies
libncurses-dev

## Usage
```console
$ clang -lncurses -o client client.c
$ ./client <ip>
```
You should be able to use the compiler of your choosing.
