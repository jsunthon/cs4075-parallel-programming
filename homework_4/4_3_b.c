/**
 * Compile: gcc -g -Wall -o 4_3_b 4_3_b.c -lm -lpthread
 *
 * Run: ./4_3_b < 2k number of threads >
 *
 * Generalize this so that it works with 2k threads – 
 * odd-ranked threads are consumers and even-ranked threads are producers.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t* mutexes; //array of mutex, one for each thread
char** messages; //messages that producers produce and consumers consume
void* Send_msg(void* rank);
long thread_count;

/* Only executed by main thread */
void Get_args(int argc, char* argv[]);
void Usage(char* prog_name);

int main(int argc, char* argv[]) {
	long thread;  /* Use long in case of a 64-bit system */
	long mutex_index;
	pthread_t* thread_handles;

	Get_args(argc, argv);

	messages = malloc((thread_count / 2) * sizeof(char *)); /* only the odd ranked threads consume messages, hence thread_count / 2 */

	/* Each odd and even rank thread pair only needs to share a mutex, hence (thread_count) / 2*/
	mutexes = malloc((thread_count / 2) * sizeof(pthread_mutex_t));

	/* For each of the thread_count mutexes, unlock if even rank (producer), lock if odd rank (consumer) */
	for (mutex_index = 0; mutex_index < (thread_count / 2); mutex_index++) {
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
	free(thread_handles);

	return 0;
}

void* Send_msg(void* rank) {
	long my_rank = (long) rank;
	char* my_msg = malloc(1024 * sizeof (char));

	while (1) {
		pthread_mutex_lock(&mutexes[(my_rank / 2)]);

		/* Odd ranked threads will consume */
		if (my_rank % 2 != 0) {
			if (messages[(my_rank / 2)] != NULL) {
				printf("Consumer Thread %ld > %s\n", my_rank, messages[(my_rank / 2)]);
				pthread_mutex_unlock(&mutexes[(my_rank / 2)]);
				break;
			}
		} 
		/* Even ranked thrads will produce */
		else {
			sprintf(my_msg, "Hello to Consumer thread %ld from Producer thread %ld", my_rank + 1, my_rank);
			messages[(my_rank / 2)] = my_msg;
			pthread_mutex_unlock(&mutexes[(my_rank / 2)]);
			break;
		}
		pthread_mutex_unlock(&mutexes[(my_rank / 2)]);
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

   if (thread_count <= 0 || thread_count % 2 != 0) Usage(argv[0]);
}  /* Get_args */


/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Print a message explaining how to run the program
 * In arg:    prog_name
 */
void Usage(char* prog_name) {
   fprintf(stderr, "usage: %s < 2k number of threads >\n", prog_name);
   exit(0);
}  /* Usage */