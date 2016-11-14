/*
Exercise 5.1: Modify the matrix-vector multiplication so that
each thread uses private storage for its part of y during the for
i loop. When a thread is done computing its part of y, it should copy its
private storage into the shared variable.

Compile: gcc -g -Wall -o 5_1 5_1.c -lm -lpthread

Run: ./5_1 <number of threads> <m> <n>

m = number of rows in matrix A and size of vector y
n = number of columns in matrix A and size of vector x
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

const int MAX_THREADS = 1024;

long thread_count;
long m; //# of rows
long n; //# of columns
int** A; //matrix
int* x; //vector
int* y; //shared variable y. the matrix vector product.
void Get_args(int argc, char* argv[]);
void Usage(char* prog_name);
void* Pth_mat_vect(void* rank);
void Gen_matrix();
void Gen_vector();
void Print_vector(int* vector, char* title, int length);
void Print_matrix();

int main(int argc, char* argv[]) {
	long thread;
	pthread_t* thread_handles;

	Get_args(argc, argv);
	A = malloc(m * sizeof(int *));
	x = malloc(n * sizeof(int)); //x has n rows
	y = malloc(m * sizeof(int)); //y has m rows
	thread_handles = malloc (thread_count * sizeof ( pthread_t) );

	Gen_matrix();
	Gen_vector();
	Print_matrix();
	Print_vector(x, "Vector x", n);

	for (thread = 0; thread < thread_count; thread++) {
 		pthread_create(&thread_handles[thread], NULL, Pth_mat_vect, (void*) thread);
   	}

	for (thread = 0; thread < thread_count; thread++) {
      	pthread_join(thread_handles[thread], NULL);   	
   	}

   	Print_vector(y, "Vector y: ", m);

   	free(A);
   	free(x);
   	free(y);
   	free(thread_handles);

	return 0;
}

/* Fill out matrix A with random value from 0 to 10, exclusive*/
void Gen_matrix() {
	int i, j;
	srand(time(NULL));
	for (i = 0; i < m; i++) {
		A[i] = malloc(n * sizeof(int)); //for each row, allocate memory for n ints
		for (j = 0; j < n; j++) {
			A[i][j] = rand() % 10;
		}
	}
}

void Gen_vector() {
	srand(time(NULL));
	int i;
	for (i = 0; i < n; i++) {
		int random = rand() % 10;
		// printf("\ni: %d", i);
		// printf("\nRandom: %d", random);
		x[i] = random;
	}
}

void Print_matrix() {
	int i, j;
	srand(time(NULL) + 1);
	printf("\nMatrix A:\n");
	for (i = 0; i < m; i++) {
		printf("\n[");
		for (j = 0; j < n; j++) {
			printf(" %d", A[i][j]);
		}
		printf(" ]");
		if (i == m - 1) {
			printf("\n\n");
		}
	}
}

void Print_vector(int* vector, char* title, int length) {
	int i;
	printf("\n%s\n", title);
	for (i = 0; i < length; i++) {
		if (i == 0) {
			printf("[");
		}
		printf(" %d", vector[i]);
		if (i == length - 1) {
			printf(" ]\n\n");
		}
	}
}

void* Pth_mat_vect(void* rank) {
	long my_rank = (long) rank;
	int i, j, y_index;
	int local_m = m / thread_count;
	int my_first_row = my_rank * local_m;
	int my_last_row = (my_rank + 1) * local_m - 1;

	int* local_y = malloc(local_m * sizeof(int));
	for (y_index = 0, i = my_first_row; i <= my_last_row; i++, y_index++) {
		local_y[y_index] = 0.0;
		for (j = 0; j < n; j++) {
			local_y[y_index] += A[i][j]*x[j];
		}
		// printf("\n%d\n", local_y[y_index]);
	}
	// Print_vector(local_y, "Local y: ", local_m);

	//after local_y computed, put into shared variable y
	for (i = 0, j = my_first_row; i < local_m; i++, j++) {
		y[j] = local_y[i];
	}
	// printf("Thread %ld, finished calculating local y.", my_rank);
	return NULL;
}
/*------------------------------------------------------------------
 * Function:    Get_args
 * Purpose:     Get the command line args
 * In args:     argc, argv
 * Globals out: thread_count, n
 */
void Get_args(int argc, char* argv[]) {
   if (argc != 4) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);  
   if (thread_count <= 0 || thread_count > MAX_THREADS) Usage(argv[0]);
   m = strtol(argv[2], NULL, 10);
   n = strtol(argv[3], NULL, 10);
}  /* Get_args */


/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Print a message explaining how to run the program
 * In arg:    prog_name
 */
void Usage(char* prog_name) {
   fprintf(stderr, "usage: %s <number of threads> <m, number of rows> <n, number of columns>\n", prog_name);
   fprintf(stderr, "   m is the number of terms and should be >= 1\n");
   fprintf(stderr, "   m should be evenly divisible by the number of threads\n");
   exit(0);
}  /* Usage */