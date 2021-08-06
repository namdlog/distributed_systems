
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[]){
	// declare string variables
	char program[12];
	char signal[5];
	char * pEnd;
    
    // validate parameters
    if(argc < 3){
    	printf("Please enter the PID of the proccess as the first parameter and the Signal number in the second parameter.\n");
    	return EXIT_FAILURE;
    }

    // get and validate pid
	int pid = strtol(argv[1],&pEnd,10);
	if(!pid){
		printf("Please enter a valid PID\n");
		return EXIT_FAILURE;
    }

    // get and validade signal
	int sig = strtol(argv[2],&pEnd,10);
	if(!signal){
		printf("Please enter a valid signal code\n");
		return EXIT_FAILURE;
    }

    // validade if the PID is valid and send the signal
	int erro = kill(pid,0);
	if(erro == -1){
		printf("Process PID not found\n");
		printf("%s\n", strerror(errno));
	}else{
		printf("Sending signal...\n");
		if(!kill(pid,sig)){
			printf("Signal sent with success!\n");
		}else{
			printf("Signal failed :(\n");
			printf("%s\n", strerror(errno));
		}
	}
	
	return 0;
}