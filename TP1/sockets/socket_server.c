#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 1026
#define MAX_BUFFER 20

bool isPrime(int x){
    // iterate through numbers from 2 to root of x
    for(int d = 2; d * d <= x; d++){
        if (x % d == 0){
            return false;
        }
    }
    return true;
}


int main(int argc, char const *argv[]) {
    int SOCKET, new_socket;
    struct sockaddr_in serv_addr;
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons( PORT );

    int serv_addr_len = sizeof(serv_addr);
    
    // create a TCP socket 
    if((SOCKET = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
        printf("There was an error during socket creation\n");
        return EXIT_FAILURE;
    }

    if (bind(SOCKET, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("The was an error trying to bind\n");
        return EXIT_FAILURE;
    }

    if (listen(SOCKET, 2) < 0) {
        printf("There was an error trying to listen\n");
        return EXIT_FAILURE;
    }

    if ((new_socket = accept(SOCKET, (struct sockaddr *)&serv_addr,(socklen_t*)&serv_addr_len)) < 0) {
        printf("There was an error trying to accept\n");
        return EXIT_FAILURE;
    }
    
    char socket_msg[MAX_BUFFER];
    int received;
    while (true) {
        read(new_socket, &socket_msg, sizeof(char)*MAX_BUFFER);
        // break loop if received is 0 or i cant transform into an integer
        if(!(received = atoi(socket_msg))){
            break;
        };
        printf("Received %s, processing and responding...\n",socket_msg);
        sprintf(socket_msg, "%d%s", received, isPrime(received) ? " is prime\n" : " isn't prime\n");
        write(new_socket, &socket_msg, sizeof(char)*MAX_BUFFER);
    }

    close(SOCKET);
    return EXIT_SUCCESS;
}