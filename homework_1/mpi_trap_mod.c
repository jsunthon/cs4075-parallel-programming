/**
Compute trapezoidal rule with mpi_reduce
*/
#include <stdio.h>
#include <mpi.h>
#include "../headers/trap_helpers.h"

void Get_input(int, double*, double*, int*);

int main(void) {
	int my_rank, comm_sz, n, local_n;
	double a, b, h, local_a, local_b;
	double local_int, total_int;

	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

	Get_input(my_rank, &a, &b, &n);

	h = (b - a)/ n; //h is the same for all processes
	local_n = n/comm_sz; // # of trapezoids the same for all processes

	local_a = a + my_rank*local_n*h;
	local_b = local_a + local_n*h;
	local_int = Trap(local_a, local_b, local_n, h);

	MPI_Reduce(&local_int, &total_int, 1, MPI_DOUBLE, MPI_SUM, 0,
		MPI_COMM_WORLD);

	if (my_rank == 0) {
		printf("With n = %d trapezoids, our estimate\n", n);
		printf("of the integral from %f to %f = %.15e\n", a, b, total_int);
	}

	MPI_Finalize();
	return 0;
}

void Get_input(
	int my_rank,
	double* a_p,
	double* b_p,
	int* n_p) {

	if (my_rank == 0) {
		printf("Enter a, b, and n\n");
		scanf("%lf %lf %d", a_p, b_p, n_p);
	}
	MPI_Bcast(a_p, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(b_p, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(n_p, 1, MPI_INT, 0, MPI_COMM_WORLD);
}