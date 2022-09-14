
# Socket Programming in C
![intro](https://www.softprayog.in/wp-content/uploads/2019/01/server-clients-select.jpg)
## Introduction
This project implements a server-client system using socket programming. The server is a math calculator, which responds with calculated result of expression requested by client.

There are three different type implementations of server:

 1. `server1.c` is a single process server that can handle only one client at a
time. If another client tries to chat with the server while one client session is going on the second client will receive an error message like **"line busy!"**
2. `server2.c` is a multi-process server that forks a process whenever it
receives a new client request. Multiple clients will be able to chat with the server concurrently.
3. `server3.c` is a single process (single-threaded) server that uses select
system call to handle multiple clients concurrently.
##  Flow of Server and Client program 
![](https://user-images.githubusercontent.com/19291492/44955906-363dae80-aef6-11e8-9795-161a90f30b1e.png)

## How to Run?
### Compilation
1. `cd` to project directory
2. Compile both server and client program using the following commands
```bash
gcc server.c -o server 
gcc client.c -c client 
```
### Execution
1. Run the server's executable file using `./server <port_num_to_run_on>` 
2. Run the client's executable file using `./client <port_num_to_connect>`
3. Both client and server will print a message if the server accepts client's request for connection

**Note: replace 'server' with 'server1', 'server2' or 'server3' to run different types of server**
## Samples 

### server1
As we can see we can connect to only one client at a time, trying to connect second client throws **line busy!** error.
![server1img](https://raw.githubusercontent.com/prajapati-sumit/images/main/server1.jpg)

### server2
As we can see, two different processes (process ids are shown while shutting down) were created to handle two different clients 
![server2img](https://raw.githubusercontent.com/prajapati-sumit/images/main/server2.jpg)

### server3
There was only one server, which is used **select()** system call to handle multiple clients. The clients are connected to different sockets with the server.
![server3img](https://raw.githubusercontent.com/prajapati-sumit/images/main/server3.jpg)


## Comparison between server1, server2, server3
![comparison_table](https://raw.githubusercontent.com/prajapati-sumit/images/main/comparison.jpg)
## References
1. [Socket Man Page](https://man7.org/linux/man-pages/man2/socket.2.html)
2. [TCP Server Client in Linux ](https://youtu.be/JAzUbrl_ogY)
3. [Multiple Chat Clients: One Thread in C++](https://youtu.be/dquxuXeZXgo)
