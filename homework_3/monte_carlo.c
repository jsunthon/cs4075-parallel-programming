/**
 * Write an MPI program that uses a Monte Carlo method to estimate pi. 
 * Process 0 should read in the total number of tosses and broadcast it to the other processes. 
 * Use MPI Reduce to find the global sum of the local variable number in circle, 
 * and have process 0 print the result. You may want to use long long ints for the number
 *  of hits in the circle and the number of tosses, since both may have to be very large 
 *  to get a reasonable estimate of ⇡.
 *  
 *  Compile the program: mpicc -g -Wall -o monte_carlo monte_carlo.c
 *  Run: mpiexec -n 2 monte_carlo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

void Read_number_of_tosses(long long int* number_of_tosses, int my_rank, MPI_Comm comm);
double randfrom(double min, double max);
long long int Compute_number_in_circle(long long int number_of_tosses);

int main() {
	int comm_sz, my_rank;
	long long int number_of_tosses, number_in_circle, total_number_of_tosses, total_number_in_circle;

	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

	Read_number_of_tosses(&number_of_tosses, my_rank, MPI_COMM_WORLD);
	number_in_circle = Compute_number_in_circle(number_of_tosses);

	MPI_Reduce(&number_in_circle, &total_number_in_circle, 1, MPI_LONG_LONG,
               MPI_SUM, 0, MPI_COMM_WORLD);

	if (my_rank == 0) {
		total_number_of_tosses = number_of_tosses * comm_sz;
		double pi = 4 * (total_number_in_circle) / ((double) total_number_of_tosses);
		printf("\nPie estimate using Monte Carlo method: %f\n\n", pi);
	}

	MPI_Finalize();
	return 0;
}

void Read_number_of_tosses(long long int* number_of_tosses, int my_rank, MPI_Comm comm) {
	if (my_rank == 0) {
		printf("\nEnter the number of tosses:\n");
		scanf("%lld", number_of_tosses);
	}
	MPI_Bcast(number_of_tosses, 1, MPI_LONG_LONG, 0, comm);
}

long long int Compute_number_in_circle(long long int number_of_tosses) {
	long long int number_in_circle = 0;
	long long int toss;
	double x, y, distance_squared;
	srand(getpid() * time(NULL));

	for (toss = 0; toss < number_of_tosses; toss++) {
		x = randfrom(-1.0, 1.0);
		y = randfrom(-1.0, 1.0);
		distance_squared = pow(x, 2.0) + pow(y, 2.0);
		if (distance_squared <= 1.0) {
			number_in_circle++;
		}
	}

	return number_in_circle;
}

double randfrom(double min, double max) 
{
    double range = (max - min); 
    double div = RAND_MAX / range;
    return min + (rand() / div);
}