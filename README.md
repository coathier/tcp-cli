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

## Contribute
I went through so many iterations to get to this stage in the code, unfortunately it was a part of a bigger git repository and I had to extract it so there is no version control looking back to it.

Previously, I tried multithreading and forking to get the non-blocking IO eventially I landed on this which might look quite weird. Please feel free to contribute there isn't that much code.
