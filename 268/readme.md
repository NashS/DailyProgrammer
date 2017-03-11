# **Simple Multi-threaded server-client programs written in C++**

This is my first pair of programs to delve into network programming using sockets and syscalls available from the Linux OS kernel.

Once the source is built with the makefile, two binaries: server and client will be created.

server binary will start and run a server on the localhost, and only one process of the program should run at any time. 

client binary will start and run the client program connectin on the localhost, and multiple processes of the program may be run.

To broadcast a message across all clients connected to the server, enter "BROADCAST <MESSAGE>" as an argument.



