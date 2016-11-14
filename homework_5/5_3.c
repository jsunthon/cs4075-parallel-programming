/**
 * Write a Pthreads program that uses a Monte Carlo method to estimate pi. 
 *  
 *  Compile the program: gcc -g -Wall -o 5_3 5_3.c -lm -lpthread
 *  Run: ./5_3 <number of threads> <number of tosses>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

const int MAX_THREADS = 1024;

long long number_of_tosses;
long long number_in_circle;
long thread_count;
pthread_mutex_t mutex;

void Get_args(int argc, char* argv[]);
void Usage(char* prog_name);
void* Compute_number_in_circle();
double randfrom(double min, double max);

int main(int argc, char* argv[]) {
	long thread;
	pthread_t* thread_handles;

	Get_args(argc, argv);

	pthread_mutex_init(&mutex, NULL);

	thread_handles = malloc (thread_count * sizeof ( pthread_t) );

	for (thread = 0; thread < thread_count; thread++) {
 		pthread_create(&thread_handles[thread], NULL, Compute_number_in_circle, (void*) thread);
   	}

	for (thread = 0; thread < thread_count; thread++) {
      	pthread_join(thread_handles[thread], NULL);   	
   	}

   	long long int total_number_of_tosses = number_of_tosses * thread_count;
   	double pi = (4 * number_in_circle) / ((double) total_number_of_tosses);
	printf("\nPie estimate using Monte Carlo method: %f\n\n", pi);

   	free(thread_handles);

	return 0;
}

void* Compute_number_in_circle(void* rank) {
	long my_rank = (long) rank;
	long long int toss;
	double x, y, distance_squared;
	srand(time(NULL));
	long long int local_number_in_circle;

	for (toss = 0; toss < number_of_tosses; toss++) {
		x = randfrom(-1.0, 1.0);
		y = randfrom(-1.0, 1.0);
		distance_squared = pow(x, 2.0) + pow(y, 2.0);
		if (distance_squared <= 1.0) {
			local_number_in_circle++;
		}
	}

	pthread_mutex_lock(&mutex);
	number_in_circle += local_number_in_circle;
	pthread_mutex_unlock(&mutex);

	printf("\nThread %ld > Out of %lld number of tosses, %lld were in the circle.\n", 
		my_rank, number_of_tosses, local_number_in_circle);

	return NULL;
}

double randfrom(double min, double max) 
{
    double range = (max - min); 
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

/*------------------------------------------------------------------
 * Function:    Get_args
 * Purpose:     Get the command line args
 * In args:     argc, argv
 * Globals out: thread_count, n
 */
void Get_args(int argc, char* argv[]) {
   if (argc != 3) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);  
   if (thread_count <= 0 || thread_count > MAX_THREADS) Usage(argv[0]);
   number_of_tosses = strtoll(argv[2], NULL, 10);
}  /* Get_args */


/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Print a message explaining how to run the program
 * In arg:    prog_name
 */
void Usage(char* prog_name) {
   fprintf(stderr, "usage: %s <number of threads> <number of tosses>\n", prog_name);
   exit(0);
}  /* Usage */