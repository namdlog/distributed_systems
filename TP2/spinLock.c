#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>

long long int sum;

typedef struct {
	bool held;
} lock;

typedef struct {
	int l, r;
	char *numbers;
	lock *lock;
} thread_arg;


void acquire(lock *l){
	while(__sync_lock_test_and_set(&(l->held), true));
}

void release(lock *l){
	l->held = 0;
}

void *sumPart(void *arg){
	long long int partial_sum = 0;
	thread_arg *argument = (thread_arg *)arg;
	for(int i=argument->l;i<argument->r;i++){
		partial_sum += argument->numbers[i];
	}
	acquire(argument->lock);
	sum += partial_sum;
	release(argument->lock);
	pthread_exit(NULL);
}

int main(int argc, char *argv[]){
	srand(time(NULL));

	// verifiy parameters
	if(argc == 2){
		printf("Please enter with the number of values to be sum and the number of threads\n");
		return EXIT_FAILURE;
	}

	// validate and set mode of wait
	int n, number_of_threads;
	char *pEnd;
	if(!(n = strtol(argv[1],&pEnd,10)) || !n || !(number_of_threads = strtol(argv[2],&pEnd,10))){
		printf("Please enter a valid parameter, with n > 0\n");
		return EXIT_FAILURE;
	}

	// Fill number array of chars with random number between 0 and 100 with a signal choosen by a 'pseudo-coin'
	char *number = malloc(n * sizeof(char));
	long long int aux_sum = 0;
	for(int i=0;i<n;i++){
		bool signal = rand()%100%2;
		number[i] = ((signal ? 1 : -1) *rand()%100) + 1;
		aux_sum += number[i];
	}

	pthread_t *threads = malloc(number_of_threads * sizeof(pthread_t));
	thread_arg *thread_args = malloc(number_of_threads * sizeof(thread_arg));
	// Set the lock
	lock *l = (lock *)malloc(sizeof(lock));
	l->held = 0;

	// Create threads and save arguments
	int left_index = 0, right_index = 0;
	for(int i=0;i<number_of_threads;i++){
		thread_arg arg;
		if(i == number_of_threads-1){
			left_index = right_index;
			right_index  = n;
		}else{
			left_index = (n/number_of_threads)*i;
			right_index = (n/number_of_threads)*(i+1);
		}
		arg.l = left_index;
		arg.r = right_index;
		arg.numbers = number;
		arg.lock = l;
		thread_args[i] = arg;
	}
	
	// Wait for them doing their jobs
	clock_t begin = clock();
	for(int i=0; i<number_of_threads; i++){
		pthread_create(&(threads[i]), NULL, sumPart, &(thread_args[i]));
	}
	for(int i=0; i<number_of_threads; i++){
		pthread_join(threads[i], NULL);
	}
	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("%d\t%d\t%lf\n",n,number_of_threads,time_spent);

	// Free heap memory
	free(number);
	free(l);
	free(threads);
	free(thread_args);

	return EXIT_SUCCESS;
}