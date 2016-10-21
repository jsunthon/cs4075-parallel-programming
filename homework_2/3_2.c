/* This program uses MPI to calculate the prefix sums of an array.
Assumes that n is evenly divisible by the number of processes. */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void Read_n(int* n, int my_rank, MPI_Comm comm);
void Gen_data(int local_data[], int n, int local_n, int my_rank, MPI_Comm comm);
void Gen_local_prefix_sums(int local_data[], int local_prefix_sums[], int local_n);
void Print_vector(char title[], int local_data[], int local_n);
void Gen_final_prefix_sum(int prev_prefix_sum, int local_prefix_sums[], int local_n);

int main(void) {
	int n, local_n, comm_sz, my_rank, last_prefix_sum, prev_prefix_sum;
	int *local_data, *local_prefix_sums;
	int *prefix_sums = NULL;

	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

	// Read n, the total number of elements in the array
	Read_n(&n, my_rank, MPI_COMM_WORLD);

	// Populate each process's local data array
	local_n = n / comm_sz;
	local_data = malloc(local_n*sizeof(int));
	Gen_data(local_data, n, local_n, my_rank, MPI_COMM_WORLD);

	// For each process, get the prefix sums of their local_data array
	local_prefix_sums = malloc(local_n*sizeof(int));
	Gen_local_prefix_sums(local_data, local_prefix_sums, local_n);

	// Get the last element in local_prefix_sums to pass on to the next process
	last_prefix_sum = local_prefix_sums[local_n - 1];

	int last_total_prefix_sum;
	MPI_Scan(&last_prefix_sum, &last_total_prefix_sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

	int previous_proc_last_prefix_sum = last_total_prefix_sum - (local_prefix_sums[local_n - 1]);

	for (int i = 0; i < local_n; i++) {
		local_prefix_sums[i] += previous_proc_last_prefix_sum;
	}

	if (my_rank == 0) {
 		prefix_sums = malloc(n * sizeof(int));
      	MPI_Gather(local_prefix_sums, local_n, MPI_INT, prefix_sums, local_n,
         			MPI_INT, 0, MPI_COMM_WORLD);
      	Print_vector("Prefix sums:", prefix_sums, n);
   	} else {
      	MPI_Gather(local_prefix_sums, local_n, MPI_INT, prefix_sums, local_n,
        			MPI_INT, 0, MPI_COMM_WORLD); 
   	}

   	free(local_data);
   	free(local_prefix_sums);
   	free(prefix_sums);	
}

/* Let user decide how big n will be, the number of elements in array */
void Read_n(int* n, int my_rank, MPI_Comm comm) {
	if (my_rank == 0) {
		printf("Enter array size, n:\n");
		scanf("%d", n);
	}
	MPI_Bcast(n, 1, MPI_INT, 0, comm);
}

/* Generate n numbers randomly between 0 and 9 and scatter them to all 
other processor's local_data array */
void Gen_data(int local_data[], int n, int local_n, int my_rank, MPI_Comm comm) {
	int *data = NULL;
	srand(time(NULL));
	if (my_rank == 0) {
		int i;
		data = malloc(n*sizeof(int));
		for (i = 0; i < n; i++) {
			data[i] = rand() % 10;
		}
		Print_vector("Array:", data, n);
		MPI_Scatter(data, local_n, MPI_INT, local_data, local_n,
					MPI_INT, 0, comm);
		free(data);
	} else {
		MPI_Scatter(data, local_n, MPI_INT, local_data, local_n,
					MPI_INT, 0, comm);
	}
}

/* Print out a vector */
void Print_vector(char title[], int local_data[], int local_n) {
	printf("%s [", title);
	for (int i = 0; i < local_n; i++) {
		printf("%d", local_data[i]);
		if (i != (local_n - 1)) {
			printf(", ");
		}
	}
	printf("]\n");
}

/* Calculate the prefix sums of this processor's local_data array */
void Gen_local_prefix_sums(int local_data[], int local_prefix_sums[], int local_n) {
	int sum = 0;
	for (int i = 0; i < local_n; i++) {
		sum += local_data[i];
		local_prefix_sums[i] = sum;
	}
}

/* Calculate each processor's final prefix sums by adding the last prefix sum
from previous processor to each element in this processor's prefix sum array */
void Gen_final_prefix_sum(int prev_prefix_sum, int local_prefix_sums[], int local_n) {
	for (int i = 0; i < local_n; i++) {
		local_prefix_sums[i] += prev_prefix_sum;
	}
}
