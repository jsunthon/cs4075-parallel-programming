#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "../headers/histogram.h"


/* 1. process 0 will receive an array of ints, each tell the number of x's to print out. 2.
each process must take an equally portion amount of ints, and for each int, put the sum into 
some position in an array. this array needs to be shared and concacated among all processes.
process 0 will be the one to scatter the data to all the other processes.
*/

void Read_vector(double[], int, int, char[], int, MPI_Comm);
void Get_input(int, int*, int*, int*, int*);
void Gen_bin_maxes(int, int, int, int, int*);
double random_double(int, int);

int main() {
	int my_rank, comm_sz, bin_count, min_meas, max_meas, data_count, n, local_n;
	double* local_data = NULL;
	double* data = NULL;
	int* bin_maxes = NULL;
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);


	Get_input(my_rank, &bin_count, &min_meas, &max_meas, &data_count);
	printf("bin count received from process: %d, -> %d\n", my_rank, bin_count);
		// data = malloc(bin_count*sizeof(double));
	bin_maxes = malloc(bin_count*sizeof(int));
	Gen_bin_maxes(my_rank, min_meas, max_meas, bin_count, bin_maxes);
	print_bin_maxes(bin_maxes, bin_count);

		// printf("%lf", random_double(min_meas, max_meas));
	

	// //generate data points
	// Gen_data(my_rank, comm_sz)
	
	MPI_Finalize();
	return 0;
}

// void Gen_local_data(double* local_data) {

// }


void Get_input(
	int my_rank,
	int* bin_count_p,
	int* min_meas_p,
	int* max_meas_p,
	int* data_count_p) {

	if (my_rank == 0) {
		printf("Enter bin_count, min_meas, max_meas, and data_count: \n");
		scanf("%d %d %d %d", bin_count_p, min_meas_p, max_meas_p, data_count_p);
	}
	MPI_Bcast(bin_count_p, 1, MPI_INT, 0, MPI_COMM_WORLD); // send bin count to everyone
}

double random_double(int min, int max) {
	double r = (rand() + min) % max;
	return r;
}

void Gen_bin_maxes(int my_rank, int min_meas, int max_meas, int bin_count, int* bin_maxes) {
	if (my_rank == 0) {
		int i, bin_width = (max_meas - min_meas) / bin_count;

		for (i = 0; i < bin_count; i++) {
			bin_maxes[i] = min_meas + bin_width * (i + 1);
		}
	}
	MPI_Bcast(bin_maxes, 1, MPI_INT, 0, MPI_COMM_WORLD);
}

// void Read_vector(
// 	double local_a[],
// 	int local_n,
// 	int n,
// 	char vec_name[],
// 	int my_rank,
// 	MPI_Comm comm) {

// 	double* a = NULL;
// 	int i;

// 	if (my_rank == 0) {
// 		a = malloc(n*sizeof(double));
// 		printf("Enter the vector %s\n", vec_name);
// 		for (i = 0; i < n; i++)
// 			scanf("%lf", &a[i]);
// 		MPI_Scatter(a, local_n, MPI_DOUBLE, local_a, local_n,
// 			MPI_DOUBLE, 0, comm);
// 		free(a);
// 	} else {
// 		MPI_Scatter(a, local_n, MPI_DOUBLE, local_a, local_n,
// 			MPI_DOUBLE, 0, comm);
// 	}
// }