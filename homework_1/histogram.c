#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


/* 1. process 0 will receive an array of ints, each tell the number of x's to print out. 2.
each process must take an equally portion amount of ints, and for each int, put the sum into 
some position in an array. this array needs to be shared and concacated among all processes.
process 0 will be the one to scatter the data to all the other processes.
*/

void Read_vector(double[], int, int, char[], int, MPI_Comm);
void Get_input(int, int*, int*, int*, int*);
void Gen_bin_maxes(int, int, int, int*);
void print_bin_maxes(int*, int);

int main() {
	int my_rank, comm_sz, bin_count, min_meas, max_meas, data_count;
	// double data[];
	int* bin_maxes = NULL;
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

	if (my_rank == 0) {
		Get_input(my_rank, &bin_count, &min_meas, &max_meas, &data_count);
		bin_maxes = malloc(bin_count*sizeof(int));
		Gen_bin_maxes(min_meas, max_meas, bin_count, bin_maxes);
		print_bin_maxes(bin_maxes, bin_count);
	}
	
	MPI_Finalize();
	return 0;
}


void Get_input(
	int my_rank,
	int* bin_count_p,
	int* min_meas_p,
	int* max_meas_p,
	int* data_count_p) {

	printf("Enter bin_count, min_meas, max_meas, and data_count: \n");
	scanf("%d %d %d %d", bin_count_p, min_meas_p, max_meas_p, data_count_p);
}

void Gen_bin_maxes(int min_meas, int max_meas, int bin_count, int* bin_maxes) {
	int i, bin_width = (max_meas - min_meas) / bin_count;

	for (i = 0; i < bin_count; i++) {
		bin_maxes[i] = min_meas + bin_width * (i + 1);
	}
}

void print_bin_maxes(int* bin_maxes, int bin_count) {
	int i;
	for (i = 0; i < bin_count; i++) {
		printf("%d\n", *(bin_maxes + i));
	}
}


void Read_vector(
	double local_a[],
	int local_n,
	int n,
	char vec_name[],
	int my_rank,
	MPI_Comm comm) {

	double* a = NULL;
	int i;

	if (my_rank == 0) {
		a = malloc(n*sizeof(double));
		printf("Enter the vector %s\n", vec_name);
		for (i = 0; i < n; i++)
			scanf("%lf", &a[i]);
		MPI_Scatter(a, local_n, MPI_DOUBLE, local_a, local_n,
			MPI_DOUBLE, 0, comm);
		free(a);
	} else {
		MPI_Scatter(a, local_n, MPI_DOUBLE, local_a, local_n,
			MPI_DOUBLE, 0, comm);
	}
}