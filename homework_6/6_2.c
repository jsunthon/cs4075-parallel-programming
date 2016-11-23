/**
 * Compile the program: gcc -g -Wall -o 6_2 6_2.c -lm -lpthread
 * Run: ./6_2 <number of threads> <number of tasks>
 * 
 * Write a Pthreads program that implements a “task queue.” The main thread begins 
by starting a user-specified number of threads that immediately go to sleep in 
a condition wait. The main thread generates blocks of tasks to be car- ried out
by the other threads; each time it generates a new block of tasks, it awakens
a thread with a condition signal. When a thread finishes executing its block of
tasks, it should return to a condition wait. When the main thread completes 
generating tasks, it sets a global variable indicating that there will be no more
tasks, and awakens all the threads with a condition broadcast. For the sake of 
explicitness, make your tasks linked list operations.

Notes: Main thread creates a linked list, sorted. Also, specify
the number of threads in the input as well as the number of tasks.
If the number of tasks = 9, and number of threads = 3, then each thread
will perform 3 tasks/operations. These tasks can be member, delete, or insert,
e.g. check if number exists, delete a number, or insert a number. Can do any
of those 3 tasks, e.g. member, member, member. Use a read-write lock
strategy to guarantee the result is correct.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

const int MAX_THREADS = 1024;
long thread_count;
long tasks; //total number of tasks to execute
long local_tasks; //number of tasks each thread will execute
struct list_node_s* head_p;
pthread_rwlock_t rwlock;

void Get_args(int argc, char* argv[]);
void Usage(char* prog_name);
void* Exec_task(void* rank);
int Member(int value);
int Insert(int value);
int Delete(int value);

struct list_node_s {
	int data;
	struct list_node_s* next;
};

int main(int argc, char* argv[]) {
	long thread;
	pthread_t* thread_handles;
	Get_args(argc, argv);

	local_tasks = tasks / thread_count;

	/* Generate a sorted linked list of ten nodes, with data=0, data=1,..., data=9 */
	for (int i = 0; i < 10; i++) {
		Insert(i);
	}

	pthread_rwlock_init(&rwlock, NULL);

	thread_handles = malloc(thread_count * sizeof(pthread_t));

	for (thread = 0; thread < thread_count; thread++) {
		pthread_create(&thread_handles[thread], NULL, Exec_task, (void*) thread);
	}

	for (thread = 0; thread < thread_count; thread++) {
		pthread_join(thread_handles[thread], NULL);
	}

	pthread_rwlock_destroy(&rwlock);
	free(thread_handles);

	return 0;
}

void* Exec_task(void* rank) {
	long my_rank = (long) rank;
	int i, value, function_index, success;
	srand(my_rank + time(NULL));
	for (i = 0; i < local_tasks; i++) {
		value = rand() % 10; //0, 1, ..., 9
		function_index = rand() % 3; // 0, 1, or 2
		switch (function_index) {
			case 0:
				pthread_rwlock_rdlock(&rwlock);
				success = Member(value);
				if (success == 1) {
					printf("\nThread %ld > list contains %d.\n", my_rank, value);
				} else {
					printf("\nThread %ld > list does not contain %d.\n", my_rank, value);
				}
				pthread_rwlock_unlock(&rwlock);
				break;
			case 1:
				pthread_rwlock_wrlock(&rwlock);
			 	success = Insert(value);
			 	if (success == 1) {
					printf("\nThread %ld > Successfully inserted %d.\n", my_rank, value);
				} else {
					printf("\nThread %ld > Failed to insert %d.\n", my_rank, value);
				}
				pthread_rwlock_unlock(&rwlock);
				break;
			case 2:
				pthread_rwlock_wrlock(&rwlock);
				success = Delete(value);
				if (success == 1) {
					printf("\nThread %ld > Successfully deleted %d.\n", my_rank, value);
				} else {
					printf("\nThread %ld > Failed to delete %d.\n", my_rank, value);
				}
				pthread_rwlock_unlock(&rwlock);
				break;
		}
	}
	return NULL;
}

int Member(int value) {
	struct list_node_s* curr_p = head_p;

	while (curr_p != NULL && curr_p->data < value)
		curr_p = curr_p->next;

	if (curr_p == NULL || curr_p->data > value) {
		return 0;
	} else {
		return 1;
	}
}

int Insert(int value) {
	struct list_node_s* curr_p = head_p;
	struct list_node_s* pred_p = NULL;
	struct list_node_s* temp_p;

	while (curr_p != NULL && curr_p->data < value) {
		pred_p = curr_p;
		curr_p = curr_p->next;
	}

	if (curr_p == NULL || curr_p->data > value) {
		temp_p = malloc(sizeof(struct list_node_s));
		temp_p->data = value;
		temp_p->next = curr_p;
		if (pred_p == NULL) {
			head_p = temp_p;
		} else {
			pred_p->next = temp_p;
		}
		return 1;
	} else {
		return 0;
	}
}

int Delete(int value) {
	struct list_node_s* curr_p = head_p;
	struct list_node_s* pred_p = NULL;

	while (curr_p != NULL && curr_p->data < value) {
		pred_p = curr_p;
		curr_p = curr_p->next;
	}

	if (curr_p != NULL && curr_p->data == value) {
		if (pred_p == NULL) {
			head_p = curr_p->next;
			free(curr_p);
		} else {
			pred_p->next = curr_p->next;
			free(curr_p);
		}
		return 1;
	} else {
		return 0;
	}
}

/*------------------------------------------------------------------
 * Function:    Get_args
 * Purpose:     Get the command line args
 * In args:     argc, argv
 * Globals out: thread_count, tasks
 */
void Get_args(int argc, char* argv[]) {
   if (argc != 3) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);  
   if (thread_count <= 0 || thread_count > MAX_THREADS) Usage(argv[0]);
   tasks = strtol(argv[2], NULL, 10);
   if (tasks <= 0 || (tasks % thread_count != 0)) Usage(argv[0]);
}  /* Get_args */


/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Print a message explaining how to run the program
 * In arg:    prog_name
 */
void Usage(char* prog_name) {
   fprintf(stderr, "usage: %s <number of threads> <number of tasks>\n", prog_name);
   fprintf(stderr, "   number of tasks should be evenly divisible by the number of threads\n");
   exit(0);
}  /* Usage */