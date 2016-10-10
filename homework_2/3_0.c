/*
Give a parallel program for adding two vectors x and y. 
Process 0 lets the user input the order, x and y, and then print x, y and x+y. Your program should have Read_vector and Print_vector functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

void Read_n(int* n, int my_rank, MPI_Comm comm);
void Read_vector(int x[], int y[], int local_vec1[], int local_vec2[], int local_n,
	int my_rank, int comm_sz, MPI_Comm comm);
void compute_local_sum(int local_vec1[], int local_vec2[], 
	int local_sum[], int local_n);
void Print_input_vector(int x[], int y[], int n);
void Print_vector_sum(int local_vector_sum[], int local_n, int n, 
	int my_rank, MPI_Comm comm);

int main(void) {
	int n, local_n;
	int *x, *y, *local_vec1, *local_vec2;
	int *local_vector_sum;
	int comm_sz, my_rank;

	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

	//Read n (order of vector), and broadcast to all other process
	Read_n(&n, my_rank, MPI_COMM_WORLD);

	//Now that each proc have n, calc local_n
	local_n = n / comm_sz;

	/*get the portion of the first vector this proc is
	responsible for*/
	local_vec1 = malloc(local_n * sizeof(int));
	local_vec2 = malloc(local_n * sizeof(int));
	x = malloc(local_n * comm_sz * sizeof(int));
	y = malloc(local_n * comm_sz * sizeof(int));
	Read_vector(x, y, local_vec1, local_vec2, local_n, my_rank, comm_sz, MPI_COMM_WORLD);

	/* Use local_vec1 and local_vect2 to compute local_vector_sum */
	local_vector_sum = malloc(local_n * sizeof(int));
	compute_local_sum(local_vec1, local_vec2, local_vector_sum, local_n);

	if (my_rank == 0) {
		Print_input_vector(x, y, n);
	}

	Print_vector_sum(local_vector_sum, local_n, n, my_rank, MPI_COMM_WORLD);

	free(local_vec1);
	free(local_vec2);
	free(local_vector_sum);

	MPI_Finalize();
	return 0;
}

void Read_n(int* n, int my_rank, MPI_Comm comm) {
	if (my_rank == 0) {
		printf("Enter order of vector, n:\n");
		scanf("%d", n);
	}
	MPI_Bcast(n, 1, MPI_INT, 0, comm);
}

void Read_vector(int x[], int y[], int local_vec1[], int local_vec2[], int local_n,
	int my_rank, int comm_sz, MPI_Comm comm) {
	int i, j;

	if (my_rank == 0) {
		printf("Enter vector x: \n");
		for (i = 0; i < local_n * comm_sz; i++) {
			scanf("%d", &x[i]);
		}
		MPI_Scatter(x, local_n, MPI_INT, local_vec1, local_n,
			MPI_INT, 0, comm);

		printf("Enter vector y: \n");
		for (j = 0; j < local_n * comm_sz; j++) {
			scanf("%d", &y[j]);
		}
		MPI_Scatter(y, local_n, MPI_INT, local_vec2, local_n,
			MPI_INT, 0, comm);
	} else {
		MPI_Scatter(x, local_n, MPI_INT, local_vec1, local_n,
			MPI_INT, 0, comm);
		MPI_Scatter(y, local_n, MPI_INT, local_vec2, local_n,
			MPI_INT, 0, comm);
	}
}

void compute_local_sum(int local_vec1[], int local_vec2[], int local_sum[], int local_n) {
	int i;
	for (i = 0; i < local_n; i++) {
		local_sum[i] = local_vec1[i] + local_vec2[i];
	}
}

void Print_input_vector(int x[], int y[], int n) {
	int i, j;
	//print vector x
	printf("Vector x:\n");
	for (i = 0; i < n; i++) {
		printf("%d", x[i]);
		if (i != (n - 1)) {
			printf(", ");
		}
	}

	//print vector y
	printf("\nVector y:\n");
	for (j = 0; j < n; j++) {
		printf("%d", y[j]);
		if (j != (n - 1)) {
			printf(", ");
		}
	}
}

void Print_vector_sum(int local_vector_sum[], int local_n, int n, int my_rank, MPI_Comm comm) {
	int* vector_sum = NULL;
	int k;

	if (my_rank == 0) {
		vector_sum = malloc(n * sizeof(int));
		MPI_Gather(local_vector_sum, local_n, MPI_INT, vector_sum,
			local_n, MPI_INT, 0, comm);
		printf("\nVector sum:\n");

		for (k = 0; k < n; k++) {
			printf("%d", vector_sum[k]);
			if (k != (n - 1)) {
			printf(", ");
			}
		}
		printf("\n");
		free(vector_sum);
	} else {
		MPI_Gather(local_vector_sum, local_n, MPI_INT, vector_sum,
			local_n, MPI_INT, 0, comm);
	}
}
