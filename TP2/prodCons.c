#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>
#include <unistd.h>
#include "common.h"

#define THRESHOLD 100000

int *shared_memory;
int n_consumed;
int n;

sem_t mutex, full, empty;

void addResource(int *vec){
	// Iterare through shared memory where vec[i] = 0 to put a resource
	for(int i=0;i<n;i++){
		if(!vec[i]){
			vec[i] = getRandomNumber();
			return;
		}
	}
	return;
}

int removeResource(int *vec){
	// Find a space not zero in array
	for(int i=0;i<n;i++){
		if(vec[i]){
			int aux = vec[i];
			vec[i] = 0;
			return aux;
		}
	}
	return true;
}

void free_threads(){
	// Free other blocked threads after read all needed values
	sem_post(&mutex);
	sem_post(&full);
	sem_post(&empty);
}

void *producer(void *arg){
	// Produces a random number and add it in array
	while(n_consumed < THRESHOLD){
		sem_wait(&empty);
		sem_wait(&mutex);
		addResource(shared_memory);
		sem_post(&mutex);
		sem_post(&full);
	}
	free_threads();
	pthread_exit(NULL);
}

void *consumer(void *arg){
	// Consumes a value in the array
	while(n_consumed < THRESHOLD){
		sem_wait(&full);
		sem_wait(&mutex);
		if(n_consumed >= THRESHOLD){
			// Finish execution
			free_threads();
			pthread_exit(NULL);
		}
		int read = removeResource(shared_memory);
		printf("Number %d ",read);
		if(isPrime(read)){
			printf("is prime\n");
		}else{
			printf("isn't prime\n");
		}
		n_consumed++;
		sem_post(&mutex);
		sem_post(&empty);
	}
	free_threads();
	pthread_exit(NULL);
}

int main(int argc, char *argv[]){
	srand(time(NULL));

	// Validate arguments
	if(argc < 4){
		printf("Please enter 3 parameters, the number of producers, the number of consumers and the number of values\n");
		return EXIT_FAILURE;
	}

	char *pEnd;
	int np, nc;
	if(!(n = strtol(argv[1],&pEnd,10)) || !(np = strtol(argv[2],&pEnd,10)) || !(nc = strtol(argv[3],&pEnd,10)) || np <= 0 || nc <= 0){
		printf("Please enter a valid parameter, with n > 0, np > 0 and nc > 0\n");
		return EXIT_FAILURE;
	}

	// Declare semaphores
	sem_init(&empty, 0, n);
	sem_init(&full, 0, 0);
	sem_init(&mutex, 0, 1);

	// Fill dyanmic memories
	shared_memory = malloc(n * sizeof(int));
	pthread_t *threads_producers = malloc(np * sizeof(pthread_t));
	pthread_t *threads_consumers = malloc(nc * sizeof(pthread_t));

	// Create threads
	int voltei_p,voltei_c;
	int max_between_nc_np = nc <= np ? np : nc;
	clock_t begin = clock();
	for(int i=0;i<max_between_nc_np;i++){
		if(i<np) pthread_create(&threads_producers[i], NULL, producer, NULL);
		if(i<nc) pthread_create(&threads_consumers[i], NULL, consumer, NULL);
	}

	// Wait for them finishes
	printf("oeeoie");
	for(int i=0;i<np;i++){
		pthread_join(threads_producers[i], NULL);
	}
	for(int i=0;i<nc;i++){
		pthread_join(threads_consumers[i], NULL);
	}

	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("Time spent:\t%lf\n",time_spent);
	
	// Free memory
	free(shared_memory);
	free(threads_producers);
	free(threads_consumers);
	return EXIT_SUCCESS;
}