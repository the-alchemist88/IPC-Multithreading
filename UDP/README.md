# Client/Server Implementation(UDP)

A simple implementation of the UDP (User Datagram Protocol) client/server program in the C programming language. 
In this case, client sends message to server and server echoes back.

# Steps to compile the program 

1) $gcc server.c -o server
2) $gcc client.c -o client
or
2) $gcc client_connected.c -o client_connected

# Steps to run the program

1) $./server 12345
2) $./client 12345
or
2) $./client_connected 12345

For threaded version compile with -lpthread option
(client sends message each 1 sec and server receives--press Ctrl+C to halt)

1) $gcc server_threaded.c -o server_threaded -lpthread
2) $gcc client_threaded.c -o server_threaded -lpthread





