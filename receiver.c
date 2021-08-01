#include <stdio.h>
#include <signal.h>
#include <stdlib.h> 
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

void sig_handler(int sig){
	// handle signal
	printf("d %d\n",sig);
	if(sig == SIGUSR1){
		printf("30, Got it!\n");
	}else if(sig == SIGUSR2){
		printf("31! Ok...\n");
	}else if(sig == SIGINT){
		printf("Received 2 x.x Exiting...\n");
		exit(EXIT_SUCCESS);
	}else{
		printf("%d is not a valid signal\n");
	}
}

int main(int argc, char *argv[]){
	// validate parameters
	if(argc == 1){
		printf("Please pass a valid mode for the first parameter: 'busy' or 'blocking'\n");
		return EXIT_FAILURE;
	}

	// validate and set mode of wait
	bool blocking_wait = 0;
	if(!strcmp(argv[1],"busy")){
		blocking_wait = 0;
	}else if(!strcmp(argv[1],"blocking")){
		blocking_wait = 1;
	}else{
		printf("Please pass a valid mode for the first parameter: 'busy' or 'blocking'\n");
		return EXIT_FAILURE;
	}

	// declare signal handlers
	signal(SIGUSR1, sig_handler);
	signal(SIGUSR2, sig_handler);
	signal(SIGINT, sig_handler);

	// wait
	while(true){
		if(blocking_wait){
			pause();
		}
	}
	
	return EXIT_SUCCESS;
}