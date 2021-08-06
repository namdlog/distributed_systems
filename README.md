# First practival assigments in Distributed Systems subject
## Signals

In signals we have a sender and a receiver proccess that will work together. The work was all write in C, so first compile both with gcc:

~~~
gcc receiver.c -o receiver
gcc sender.c -o sender
~~~

Then execute receiver with a parameter:
* 'busy' - busy wait (while only)
* 'blocking' - blocking wait (while with pause waiting for a signal)

To get the PID of the receiver process, try to execute the ps command on terminal to get the number of PID target
After that, execute sender passing the first parameter as the PID and the second parameter with:
* 2 - To exit the program
* 30 - Receive a message
* 31 - Receive a message

## Pipes

In pipes we will execute one proccess that will fork and create a child proccess to bind the reader side of the pipe, on the other side parent proccess will handle the writer side. So first comile with gcc:

~~~
gcc pipes.c -o pipes
~~~

Pipes needs only one parameter that is an integer specifying the order of random numbers that will be send in the pipe to the reader to proccess and verify if is prime, until receives a 0 after the n numbers.

For example:

~~~
./pipes 5
~~~

## Sockets

With sockets we have a server and a client that will conect each other by a TCP socket listening in the port 1026.
The server needs to be compile and executed only. The client needs to be compile and execute with a integer parameter, specifying the order of numbers that will be send after connection established successfully.

~~~
gcc socket_client.c -o socket_client
gcc socket_server.c -o socket_server
~~~

After executing socket_server, execute socket_client with a integer as the first parameter

~~~
./socket_client 5
~~~
