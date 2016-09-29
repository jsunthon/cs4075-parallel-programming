#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "../headers/histogram.h"

void Get_input(int* bin_count_p,
	int* min_meas_p,
	int* max_meas_p,
	int* data_count,
    int my_rank, 
    int comm_sz, 
    MPI_Comm comm);
void Gen_bin_maxes(int, int, int, int, int*);
double random_double(int, int);
void Gen_data(double local_data[], int data_count, int local_data_count,
 int min, int max, int my_rank, MPI_Comm comm);
void Init_bin_counts(int* bin_counts, int bin_count);
void Alloc_bins(int bin_count, int bin_counts[], int local_data_count, double* local_data, int min, int* bin_maxes);

int main() {
	int my_rank, comm_sz, bin_count, min_meas, max_meas, data_count, local_data_count;
	int* bin_maxes;
	double* local_data;
	int* bin_counts;
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

	Get_input(&bin_count, &min_meas, &max_meas, &data_count, my_rank, comm_sz, MPI_COMM_WORLD);
	local_data_count = data_count / comm_sz;
	// printf("bin count received from process: %d, -> %d\n", my_rank, bin_count);
	bin_maxes = malloc(bin_count*sizeof(int));
	Gen_bin_maxes(my_rank, min_meas, max_meas, bin_count, bin_maxes);
	// printf("Bin maxes:\n");
	// print_bin_maxes(bin_maxes, bin_count);
	// printf("\n");
	// printf("Data count for process %d: %d\n", my_rank, data_count);
	local_data = malloc(local_data_count*sizeof(double));
	Gen_data(local_data, data_count, local_data_count, min_meas, max_meas, my_rank, MPI_COMM_WORLD);
	printf("local data for rank: %d\n", my_rank);
	print_data(local_data, local_data_count);
	
	bin_counts = malloc(bin_count*sizeof(int));
	Init_bin_counts(bin_counts, bin_count);
	Alloc_bins(bin_count, bin_counts, local_data_count, local_data, min_meas, bin_maxes);
	// printf("bin counts for rank: %d\n", my_rank);
	// print_bin_maxes(bin_counts, bin_count);

	MPI_Finalize();
	return 0;
}

void Get_input(
	int* bin_count_p,
	int* min_meas_p,
	int* max_meas_p,
	int* data_count,
    int my_rank, 
    int comm_sz, 
    MPI_Comm comm) {

	if (my_rank == 0) {
		printf("Enter bin_count, min_meas, max_meas, and data_count: \n");
		scanf("%d %d %d %d", bin_count_p, min_meas_p, max_meas_p, data_count);
	}

	MPI_Bcast(bin_count_p, 1, MPI_INT, 0, comm); // send bin count to everyone
	MPI_Bcast(min_meas_p, 1, MPI_INT, 0, comm);
	MPI_Bcast(max_meas_p, 1, MPI_INT, 0, comm);
	MPI_Bcast(data_count, 1, MPI_INT, 0, comm);
}

void Gen_bin_maxes(int my_rank, int min_meas, int max_meas, int bin_count, int* bin_maxes) {
	if (my_rank == 0) {
		int i, bin_width = (max_meas - min_meas) / bin_count;

		for (i = 0; i < bin_count; i++) {
			bin_maxes[i] = min_meas + bin_width * (i + 1);
		}
	}
	MPI_Bcast(bin_maxes, bin_count, MPI_INT, 0, MPI_COMM_WORLD);
}

void Gen_data(double local_data[], int data_count, int local_data_count,
 int min, int max, int my_rank, MPI_Comm comm) {
 	double* data = NULL;
 	int i;

	if (my_rank == 0) {
		data = malloc(data_count*sizeof(double));

		for (i = 0; i < data_count; i++) {
			data[i] = random_double(min, max);
		}
		 MPI_Scatter(data, local_data_count, MPI_DOUBLE, local_data, local_data_count,
            MPI_DOUBLE, 0, comm);
	} else {
		 MPI_Scatter(data, local_data_count, MPI_DOUBLE, local_data, local_data_count,
            MPI_DOUBLE, 0, comm);
	}
}

double random_double(int min, int max) {
	double r = (rand() + min) % max;
	return r;
}

void Init_bin_counts(int* bin_counts, int bin_count) {
	int i;
	for (i = 0; i < bin_count; i++) {
		bin_counts[i] = 0;
	}
} 

void Alloc_bins(
	int bin_count,
	int bin_counts[], 
	int local_data_count,
	double* local_data, 
	int min, 
	int* bin_maxes) {

	int i, j;

	for (i = 0; i < local_data_count; i++) {
		int data = local_data[i];
		for (j = 0; j < bin_count; j++) {
			int bin = j;
			if (j == 0) {
				if (data >= min && data < bin_maxes[bin]) {
					bin_counts[bin]++;
				}	
			} else if (data >= bin_maxes[j-1] && data < bin_maxes[j]) {
				bin_counts[bin]++;
			}
		}
	}
}