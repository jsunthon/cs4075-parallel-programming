/**
 * Description: Implement matrix-vector multiplication using block distribution of the matrix. 
 * You can have process 0 read the matrix and the vector.
*  Assume the matrix is square of order n and that n is evenly divisible by comm_sz.
 * Compile the program: mpicc -g -Wall -o matrix_vector_mult matrix_vector_mult.c
 * Run: mpiexec -n 2 matrix_vector_mult
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

void Read_n(int* n, int my_rank, MPI_Comm comm);
void Read_vector(int matrix[], int vector[], int n, int local_matrix[],
	int local_n, int my_rank, int comm_sz, MPI_Comm comm);
void Compute_local_matrix_vector_product(int local_matrix[], int local_n, int vector[],
	int n, int local_matrix_vector_product[], int local_row_count);
void Print_matrix_vector_product(int local_matrix_vector_product[], int local_n,
	int n, int my_rank, MPI_Comm comm);

int main(void) {
	int n, local_n;
	int *matrix, *vector, *local_matrix, *local_matrix_vector_product;
	int comm_sz, my_rank;

	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

	Read_n(&n, my_rank, MPI_COMM_WORLD);

	local_n = (n*n) / comm_sz;

	matrix = malloc((n*n)*sizeof(int));
	local_matrix = malloc(local_n*sizeof(int));
	vector = malloc(n*sizeof(int));
	Read_vector(matrix, vector, n, local_matrix, local_n, my_rank, comm_sz, MPI_COMM_WORLD);

	local_matrix_vector_product = malloc((n / comm_sz)*sizeof(int));
	Compute_local_matrix_vector_product(local_matrix, local_n, vector, n, 
		local_matrix_vector_product, (n/comm_sz));
	Print_matrix_vector_product(local_matrix_vector_product, (n / comm_sz), n, my_rank,
		MPI_COMM_WORLD);
	
	free(matrix);
	free(vector);
	free(local_matrix);
	free(local_matrix_vector_product);

	MPI_Finalize();
	return 0;
}

void Read_n(int* n, int my_rank, MPI_Comm comm) {
	if (my_rank == 0) {
		printf("Enter order of matrix, n:\n");
		scanf("%d", n);
	}
	MPI_Bcast(n, 1, MPI_INT, 0, comm);
}

void Read_vector(int matrix[], int vector[], int n, int local_matrix[],
	int local_n, int my_rank, int comm_sz, MPI_Comm comm) {

	int i, j;

	if (my_rank == 0) {
		printf("Enter values for the matrix of size %d x %d:\n", n, n);
		for (i = 0; i < n*n; i++) {
			scanf("%d", &matrix[i]);
		}

		MPI_Scatter(matrix, (n*n) / comm_sz, MPI_INT, local_matrix, (n*n) / comm_sz,
			MPI_INT, 0, comm);

		printf("Enter values for the vector x:\n");
		for (j = 0; j < n; j++) {
			scanf("%d", &vector[j]);
		}
	} else {
			MPI_Scatter(matrix, (n*n) / comm_sz, MPI_INT, local_matrix, (n*n) / comm_sz,
			MPI_INT, 0, comm);
	}
	MPI_Bcast(vector, n, MPI_INT, 0, comm);
}

void Compute_local_matrix_vector_product(int local_matrix[], int local_n, int vector[],
	int n, int local_matrix_vector_product[], int local_row_count) {
	int row_index = 0;
	int vector_index = 0;
	int sum = 0;
	int matrix_index;

	for (matrix_index = 0; matrix_index < local_n; matrix_index++) {
		sum += local_matrix[matrix_index] * vector[vector_index];
		vector_index++;

		/*
		If at the last element in a row, place the dot product in the apporipriate position in
		local_matrix_vector_product, then re-init vector_index to start at the beginning of vector
		 and re-init sum to start performing a new dot product to be placed later.
		 */
		int nextIndex = matrix_index + 1;
		if (nextIndex % n == 0) {
			local_matrix_vector_product[row_index] = sum;
			row_index++;
			vector_index = 0;
			sum = 0;
		}
	}
}

void Print_matrix_vector_product(int local_matrix_vector_product[], int local_n,
	int n, int my_rank, MPI_Comm comm) {
	int* matrix_vector_product = NULL;
	int k;

	if (my_rank == 0) {
		matrix_vector_product = malloc(n * sizeof(int));
		MPI_Gather(local_matrix_vector_product, local_n, MPI_INT, matrix_vector_product,
			local_n, MPI_INT, 0, comm);
		printf("\nMatrix vector product:\n");

		for (k = 0; k < n; k++) {
			printf("%d", matrix_vector_product[k]);
			if (k != (n - 1)) {
				printf(", ");
			}
		}
		printf("\n");
		free(matrix_vector_product);
	} else {
		MPI_Gather(local_matrix_vector_product, local_n, MPI_INT, matrix_vector_product,
			local_n, MPI_INT, 0, comm);
	}
}
