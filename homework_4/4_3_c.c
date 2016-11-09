/**
 * Compile: gcc -g -Wall -o 4_3_c 4_3_c.c -lm -lpthread
 *
 * Run: ./4_3_c < number of threads >
 *
 * Generalize this so that each thread is both a producer and a consumer. 
 * For example, suppose that thread q “sends” a message to thread (q + 1) mod t 
 * and “receives” a message from thread (q − 1 + t) mod t, where t is the total number of threads.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t* mutexes; //array of mutex, one for each thread
char** messages; //messages that producers produce and consumers consume
void* Send_msg(void* rank);
int* messages_available;
long thread_count;

/* Only executed by main thread */
void Get_args(int argc, char* argv[]);
void Usage(char* prog_name);

int main(int argc, char* argv[]) {
	long thread;  /* Use long in case of a 64-bit system */
	long mutex_index;
	pthread_t* thread_handles;

	Get_args(argc, argv);

	messages = malloc(thread_count * sizeof(char *));
	mutexes = malloc(thread_count * sizeof(pthread_mutex_t));

	messages_available = malloc(thread_count * sizeof(int));
	for (int i = 0; i < thread_count; i++) {
		messages_available[i] = 0;
	}

	for (mutex_index = 0; mutex_index < thread_count; mutex_index++) {
		pthread_mutex_init(&mutexes[mutex_index], NULL); //every mutex is unlocked initially.
	}

	thread_handles = malloc (thread_count * sizeof ( pthread_t) ); //allocate mem for thread_handles arr

	for (thread = 0; thread < thread_count; thread++) {
 		pthread_create(&thread_handles[thread], NULL, Send_msg, (void*) thread);
   	}

   	for (thread = 0; thread < thread_count; thread++) {
      	pthread_join(thread_handles[thread], NULL);   	
   	}

	free(messages);
	free(mutexes);
	free(messages_available);
	free(thread_handles);

	return 0;
}

void* Send_msg(void* rank) {
	long my_rank = (long) rank;
	long source = (my_rank - 1 + thread_count) % thread_count;
	long dest = (my_rank + 1) % thread_count;
	char* my_msg = malloc(1024 * sizeof (char));
	while (1) {
		pthread_mutex_lock(&mutexes[dest]);
		// printf("my_rank: %ld\n", my_rank);
		sprintf(my_msg, "Hello to thread %ld from thread %ld", dest, my_rank);
		// printf("my_rank: %ld finished sending msg to thread %ld.\n", my_rank, dest);
		messages[dest] = my_msg;
		messages_available[dest] = 1;
		pthread_mutex_unlock(&mutexes[dest]);
		break;
	}

	while (1) {
		pthread_mutex_lock(&mutexes[my_rank]);
		// printf("my_rank: %ld, checking if message is avail. \n", my_rank);
		if (messages_available[my_rank] == 1) {
			// printf("myrank: %ld ; not null\n", my_rank);
			printf("Thread %ld > %s\n", my_rank, messages[my_rank]);
			pthread_mutex_unlock(&mutexes[my_rank]);
			break;
		}
		pthread_mutex_unlock(&mutexes[my_rank]);
	}

	return NULL;
}

/*------------------------------------------------------------------
 * Function:    Get_args
 * Purpose:     Get the command line args
 * In args:     argc, argv
 * Globals out: thread_count, n
 */
void Get_args(int argc, char* argv[]) {
   if (argc != 2) Usage(argv[0]);

   thread_count = strtoll(argv[1], NULL, 10);

   if (thread_count <= 0) Usage(argv[0]);
}  /* Get_args */


/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Print a message explaining how to run the program
 * In arg:    prog_name
 */
void Usage(char* prog_name) {
   fprintf(stderr, "usage: %s < number of threads > 0 >\n", prog_name);
   exit(0);
}  /* Usage */