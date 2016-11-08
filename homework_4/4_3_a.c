/**
 * Compile: gcc -g -Wall -o 4_3_a 4_3_a.c -lm -lpthread
 *
 * Run: ./4_3_a
 *
 * Write a Pthreads program that implements this version of 
 * producer-consumer synchronization with two threads. Thread 0 
 * is the consumer, and thread 1 is the producer. Thread 0 only consumes
 * the message once thread 1 produces it.
 *
 * Notes: I don't allow inputing of the number of threads, since it's fixed
 * at 2. The number of messages is user choice though.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t mutex;
int message_available; //indicates if there's message to consume
char** messages;
void* Send_msg(void* rank);

int main(void) {
	pthread_t* thread_handles;
	long thread;
	long thread_count = 2; //this program uses only two threads
	message_available = 0; //intially, no mesage available

	messages = malloc(1 * sizeof(char *)); //only one message in the messages arr

	pthread_mutex_init(&mutex, NULL);
	thread_handles = malloc (thread_count * sizeof ( pthread_t) ); //allocate mem for thread_handles arr

	for (thread = 0; thread < thread_count; thread++) {
 		pthread_create(&thread_handles[thread], NULL, Send_msg, (void*) thread);
   	}

   	for (thread = 0; thread < thread_count; thread++) {
      	pthread_join(thread_handles[thread], NULL);   	
   	}

   	free(messages);
   	free(thread_handles);

   	return 0;
}

void* Send_msg(void* rank) {
	long my_rank = (long) rank;
	char* my_msg = malloc(1024 * sizeof (char));

	while (1) {
		pthread_mutex_lock(&mutex);
		/* thread 0 is the consumer */
		if (my_rank == 0) {
			if (message_available) {
				printf("Consumer Thread %ld > %s\n", my_rank, messages[my_rank]);
				pthread_mutex_unlock(&mutex);
				break;
			}
		} 
		/* thread 1 is the producer */
		else {
			sprintf(my_msg, "Hello to Consumer thread %d from Producer thread %ld", 0, my_rank);
			messages[0] = my_msg;
			message_available = 1;
			pthread_mutex_unlock(&mutex);
			break;
		}
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}