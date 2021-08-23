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

int main(int argc, char const *argv[]) {
    int n;
    char *pEnd;

    // validate parameters
    if(argc < 2 || !(n = strtol(argv[1],&pEnd,10)) || !n){
        printf("Enter a valid number n > 0 of prime numbers as a parameter\n");
        return EXIT_FAILURE;
    }

    // generate seed
    srand(time(NULL));
    
    int SOCKET;
    struct sockaddr_in serv_addr;

    // set port and server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    // try to connect socket
    if((SOCKET = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
        printf("There was an error during socket creation\n");
        return EXIT_FAILURE;
    }

    if (connect(SOCKET, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("There was an error during client socket conection\n");
        return EXIT_FAILURE;
    }

    char number_send[MAX_BUFFER],number_received[MAX_BUFFER];
    int number = 0;

    // send prime numbers and read the result
    while(n--) {
        number += (rand()%100);
        sprintf(number_send, "%d", number);
        write(SOCKET, &number_send, sizeof(char)*MAX_BUFFER);
        read(SOCKET, &number_received, sizeof(char)*MAX_BUFFER);
        printf("Response: %s\n",number_received);
    }

    number = 0;
    sprintf(number_send, "%d", number);
    write(SOCKET, &number_send, sizeof(char)*MAX_BUFFER);
    read(SOCKET, &number_send, sizeof(char)*MAX_BUFFER);

    // close connection
    close(SOCKET);
    return EXIT_SUCCESS;
}