/**
 * Write a Pthreads program that implements the trapezoidal rule.
 * Use a shared variable for the sum of all the threads' computations. 
 *
 * USE SEMAPHORES
 *
 * Compile the program: gcc -g -Wall -o 6_1_sem 6_1_sem.c -lm -lpthread
 * Run: ./6_1_sem <number of threads> <a> <b> <n>
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

const int MAX_THREADS = 1024;
long thread_count;
double a, b, h; //smallest value, biggest value, and size of each interval 
int n, local_n; //number of trapezoids overall, number of trapezoids globally
double total_integral; // global total_integral
sem_t sem;

void Get_args(int argc, char* argv[]);
void Usage(char* prog_name);
void* Trap(void* rank);
double f(double x);

int main(int argc, char* argv[]) {
	long thread;
	pthread_t* thread_handles;

	Get_args(argc, argv);
	// printf("a: %f, b: %f, n: %d", a, b, n);
	h = (b - a) / n;
	local_n = n / thread_count;

	thread_handles = malloc (thread_count * sizeof(pthread_t));

	sem_init(&sem, 0, 1);
	for (thread = 0; thread < thread_count; thread++) {
		pthread_create(&thread_handles[thread], NULL, Trap, (void*) thread);
	}
	sem_destroy(&sem);

	for (thread = 0; thread < thread_count; thread++) {
		pthread_join(thread_handles[thread], NULL);
	}

	printf("With n = %d trapezoids, our estimate\n", n);
	printf("of the integral from %f to %f = %.15e\n", a, b, total_integral);

	free(thread_handles);

	return 0;
}

//Compute the trapezoidal area
void* Trap(void* rank) {
	long my_rank = (long) rank;
	double estimate, x;

	double left_endpt = a + my_rank*local_n*h;
	double right_endpt = left_endpt + local_n*h;

	estimate = (f(left_endpt) + f(right_endpt)) / 2.0;
	for (int i = 1; i <= local_n - 1; i++) {
		x = left_endpt + i*h;
		estimate += f(x);
	}
	estimate = estimate*h;

	sem_wait(&sem);
	total_integral += estimate;
	sem_post(&sem);

	return NULL;
}

//Graph of y = x^2
double f(double x) {
	return x*x;
}

/*------------------------------------------------------------------
 * Function:    Get_args
 * Purpose:     Get the command line args
 * In args:     argc, argv
 * Globals out: thread_count, n
 */
void Get_args(int argc, char* argv[]) {
   if (argc != 5) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);  
   if (thread_count <= 0 || thread_count > MAX_THREADS) Usage(argv[0]);
   a = strtod(argv[2], NULL);
   b = strtod(argv[3], NULL);
   n = strtol(argv[4], NULL, 10);
   if (n <= 0 || (n % thread_count != 0)) Usage(argv[0]);
}  /* Get_args */


/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Print a message explaining how to run the program
 * In arg:    prog_name
 */
void Usage(char* prog_name) {
   fprintf(stderr, "usage: %s <number of threads> <a> <b> <n>\n", prog_name);
   fprintf(stderr, "   n is the number of trapezoids.\n");
   fprintf(stderr, "   n should be evenly divisible by the number of threads\n");
   exit(0);
}  /* Usage */