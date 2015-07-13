# tcp_socket
Simple TCP socket program

Multi-thread and semaphore were used to handle multiple clients.

In the example, Each resource has 2 instances and there are 3 kinds of resource.


Compile:

gcc server.c -o server -l pthread

gcc client.c -o client

Usage:

./server [tcp port]

./client [server ip] [tcp port]
