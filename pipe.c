#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

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

int main(int argc, char *argv[]) {
    int n;
    char *pEnd;

    // validate parameters
    if(argc < 2 || !(n = strtol(argv[1],&pEnd,10)) || !n){
        printf("Enter a valid number n > 0 of prime numbers as a parameter\n");
        return EXIT_FAILURE;
    }

    // get seed
    srand(time(NULL));

    int fd[2];

    // create a pipe
    if(pipe(fd) < 0){
      perror("pipe");
      return EXIT_FAILURE;
    }

    // fork a child proccess
    pid_t childpid;
    if((childpid = fork()) < 0){
        perror("fork");
        return EXIT_FAILURE;
    }else if(childpid == 0){
        //child

        // variables, buffer and subscribe the reader
        int nbytes;
        char number_read[MAX_BUFFER];
        close(fd[1]);
        int number;

        // while number read different from 0 verify prime test
        while(read(fd[0], number_read, sizeof(number_read)) && (number = strtol(number_read,&pEnd,10))){
            printf("The number received %d ",number);
            if(isPrime(number)){
                printf("is prime\n");
            }else{
                printf("isn't prime\n");
            }
        }

        printf("Finished reading\n");
    }else{
        //parent
        
        char number_send[MAX_BUFFER];
        close(fd[0]);
            
        // send n numbers until 0
        int number = 1;
        while(n--){
            number += (rand()%100);
            sprintf(number_send, "%d", number);
            printf("Sending number: '%s'\n", number_send);
            write(fd[1], number_send, sizeof(number_send));
            sleep(1);
        }
        // finish generating numbers
        number = 0;
        sprintf(number_send, "%d", number);
        write(fd[1], number_send, sizeof(number_send));
    }
    
    return EXIT_SUCCESS;
}  