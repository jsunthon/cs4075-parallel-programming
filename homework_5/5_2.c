/**
 * Write a Pthreads program that implements Histogram. 
 *
 *  Compile the program: gcc -g -Wall -o 5_2 5_2.c -lm -lpthread
 *  Run: ./5_2 <number of threads> <number of bins> <smallest data value> <largest data value> <number of data values>
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

const int MAX_THREADS = 1024;

long thread_count;
int bin_count, min_meas, max_meas, data_count;
double* data;
int *bin_maxes, *bin_counts;
pthread_mutex_t mutex;

void Get_args(int argc, char* argv[]);
void Usage(char* prog_name);
void Gen_bin_maxes();
void Gen_data();
void Init_bin_counts(int* bin_counts, int bin_count);
void* Alloc_bins(void* rank);
void Gen_output();

int main(int argc, char* argv[]) {

	long thread;
	pthread_t* thread_handles;

	Get_args(argc, argv);

	pthread_mutex_init(&mutex, NULL);
	thread_handles = malloc (thread_count * sizeof ( pthread_t) );

	bin_maxes = malloc(bin_count*sizeof(int));
	Gen_bin_maxes();

	data = malloc(data_count*sizeof(double));
	Gen_data();

	bin_counts = malloc(bin_count*sizeof(int));
	Init_bin_counts(bin_counts, bin_count);

	for (thread = 0; thread < thread_count; thread++) {
 		pthread_create(&thread_handles[thread], NULL, Alloc_bins, (void*) thread);
   	}

	for (thread = 0; thread < thread_count; thread++) {
      	pthread_join(thread_handles[thread], NULL);   	
   	}

   	Gen_output();

   	free(thread_handles);
   	free(data);
   	free(bin_maxes);
	free(bin_counts);

	return 0;
}

void Gen_bin_maxes() {
	int i, bin_width = (max_meas - min_meas) / bin_count;

	for (i = 0; i < bin_count; i++) {
		bin_maxes[i] = min_meas + bin_width * (i + 1);
	}
}

void Gen_data() {
 	int i;

	for (i = 0; i < data_count; i++) {
		double range = (max_meas - min_meas); 
    	double div = RAND_MAX / range;
		data[i] = min_meas + (rand() / div);
	}
}

void Init_bin_counts(int* bin_counts, int bin_count) {
	int i;
	for (i = 0; i < bin_count; i++) {
		bin_counts[i] = 0;
	}
} 

void* Alloc_bins(void* rank) {

	long my_rank = (long) rank;
	int local_data_count = data_count / thread_count;
	int my_first_i = my_rank * local_data_count;
	int my_last_i = (my_rank + 1) * local_data_count - 1;
	int i, j;

	int *local_bin_counts = malloc (bin_count * sizeof(int));
	Init_bin_counts(local_bin_counts, bin_count);

	for (i = my_first_i; i <= my_last_i; i++) {
		int value = data[i];
		for (j = 0; j < bin_count; j++) {
			int bin = j;
			if (j == 0) {
				if (value >= min_meas && value < bin_maxes[bin]) {
					local_bin_counts[bin]++;
				}	
			} else if (value >= bin_maxes[j-1] && value < bin_maxes[j]) {
				local_bin_counts[bin]++;
			}
		}
	}

	pthread_mutex_lock(&mutex);
	for (i = 0; i < bin_count; i++) {
		bin_counts[i] += local_bin_counts[i];
	}
	pthread_mutex_unlock(&mutex);

	free(local_bin_counts);

	return NULL;
}

void Gen_output() {
		int i, j;

		printf("\nHistogram data:\n ");
		printf("[");
		for (j = 0; j < data_count; j++) {
			printf("%lf", data[j]);
			if (j != (data_count - 1)) {
				printf(", ");
			}
		}
		printf("]\n");

		printf("\nHistogram results:\n ");
		for (i = 0; i < bin_count; i++) {
			int start;
			if (i == 0) {
				start = min_meas;
			} else {
				start = bin_maxes[i - 1];
			}
			printf("[%d, %d] -> %d values.", start, bin_maxes[i], bin_counts[i]);			
			printf("\n");
		}
}

/*------------------------------------------------------------------
 * Function:    Get_args
 * Purpose:     Get the command line args
 * In args:     argc, argv
 * Globals out: thread_count, n
 */
void Get_args(int argc, char* argv[]) {
   if (argc != 6) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);  
   if (thread_count <= 0 || thread_count > MAX_THREADS) Usage(argv[0]);
   //  bin_count, min_meas, max_meas, data_count
   bin_count = strtol(argv[2], NULL, 10);
   min_meas = strtol(argv[3], NULL, 10);
   max_meas = strtol(argv[4], NULL, 10);
   data_count = strtol(argv[5], NULL, 10);

}  /* Get_args */


/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Print a message explaining how to run the program
 * In arg:    prog_name
 */
void Usage(char* prog_name) {
   fprintf(stderr, "usage: %s <number of threads> <number of bins> <smallest data value> <largest data value> <number of data values>\n", prog_name);
   exit(0);
}  /* Usage */