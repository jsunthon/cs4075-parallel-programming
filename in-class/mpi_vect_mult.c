/* File:
 *    ex3.9_mpi_vect_mult.c
 *  
 * Purpose:
 *    Multiply a scalar by a vector and implement a dot product
 *
 * Compile:    
 *    mpicc -g -Wall -o mpi_vect_mult mpi_vect_mult.c
 * Run:        
 *    mpiexec -n <number of processes> mpi_vect_mult
 *
 * Input:  
 *    The order of the vectors
 *    Two vectors and a scalar
 *
 * Output:
 *    The dot product of the two vectors
 *    The product of each vector with the scalar
 *
 * IPP:  Exercise 3.9
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h> 

void Read_n(int* n_p, int* local_n_p, int my_rank, int comm_sz, 
      MPI_Comm comm);
void Check_for_error(int local_ok, char fname[], char message[], 
      MPI_Comm comm);
void Read_data(double local_vec1[], double local_vec2[], double* scalar_p,
      int local_n, int my_rank, int comm_sz, MPI_Comm comm);
void Print_vector(double local_vec[], int local_n, int n, char title[], 
      int my_rank, MPI_Comm comm);
double Par_dot_product(double local_vec1[], double local_vec2[], 
      int local_n, MPI_Comm comm);
void Par_vector_scalar_mult(double local_vec[], double scalar, 
      double local_result[], int local_n);

int main(void) {
   int n, local_n;
   double *local_vec1, *local_vec2;
   double scalar;
   double *local_scalar_mult1, *local_scalar_mult2;
   double local_dot_product, dot_product;
   int comm_sz, my_rank;
   
   MPI_Init(NULL, NULL);
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
   MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
   
   /* Print input data */
   Read_n(&n, &local_n, my_rank, comm_sz, MPI_COMM_WORLD);

   local_vec1 = malloc(local_n * sizeof(double));
   local_vec2 = malloc(local_n * sizeof(double));

   Read_data(local_vec1, local_vec2, &scalar, local_n, my_rank, comm_sz, MPI_COMM_WORLD);
   
   /* Compute and print dot product */
   local_dot_product = Par_dot_product(local_vec1, local_vec2, local_n, MPI_COMM_WORLD);
   MPI_Reduce(&local_dot_product, &dot_product, 1, MPI_DOUBLE, MPI_SUM, 0,
      MPI_COMM_WORLD);
   if (my_rank == 0) {
      printf("\nThe dot product of the two vectors: %lf\n", dot_product);
   }
   /* Compute scalar multiplication and print out result */
   local_scalar_mult1 = malloc(local_n * sizeof(double)); // alloc mem
   local_scalar_mult2 = malloc(local_n * sizeof(double)); // alloc mem
   Par_vector_scalar_mult(local_vec1, scalar, local_scalar_mult1, local_n); // compute the scalar product of vec 1 with scalar
   Par_vector_scalar_mult(local_vec2, scalar, local_scalar_mult2, local_n); // compute the scalar product of vec 2 with scalar, store in local sclar mult 1

   Print_vector(local_scalar_mult1, local_n, n, "\nScalar vector product of vector 1", my_rank, MPI_COMM_WORLD);
   Print_vector(local_scalar_mult2, local_n, n, "\nScalar vector product of vector 2", my_rank, MPI_COMM_WORLD);

   free(local_scalar_mult2);
   free(local_scalar_mult1);
   free(local_vec2);
   free(local_vec1);
   
   MPI_Finalize();
   return 0;
}

/*-------------------------------------------------------------------*/
void Check_for_error(
                int       local_ok   /* in */, 
                char      fname[]    /* in */,
                char      message[]  /* in */, 
                MPI_Comm  comm       /* in */) {
   int ok;
   
   MPI_Allreduce(&local_ok, &ok, 1, MPI_INT, MPI_MIN, comm);
   if (ok == 0) {
      int my_rank;
      MPI_Comm_rank(comm, &my_rank);
      if (my_rank == 0) {
         fprintf(stderr, "Proc %d > In %s, %s\n", my_rank, fname, 
               message);
         fflush(stderr);
      }
      MPI_Finalize();
      exit(-1);
   }
}  /* Check_for_error */


/*-------------------------------------------------------------------*/
void Read_n(int* n_p, int* local_n_p, int my_rank, int comm_sz, 
      MPI_Comm comm) {

   if (my_rank == 0) {
      printf("Enter n:\n");
      scanf("%d", n_p);
   }
   
   *(local_n_p) = *(n_p) / comm_sz;

   // printf("local n: %d", *(local_n_p));
   MPI_Bcast(n_p, 1, MPI_INT, 0, comm);
   MPI_Bcast(local_n_p, 1, MPI_INT, 0, comm);
}  /* Read_n */

/*-------------------------------------------------------------------*/
void Read_data(double local_vec1[], double local_vec2[], double* scalar_p,
      int local_n, int my_rank, int comm_sz, MPI_Comm comm) {
   double* a = NULL;
   double* b = NULL;
   int i, j;
   if (my_rank == 0){
      printf("What is the scalar?\n");
      scanf("%lf", scalar_p);
   }
   
   MPI_Bcast(scalar_p, 1, MPI_DOUBLE, 0, comm);
   
   if (my_rank == 0){
      a = malloc(local_n * comm_sz * sizeof(double));
      printf("Enter the first vector: \n");
      for (i = 0; i < local_n * comm_sz; i++) 
         scanf("%lf", &a[i]);
      MPI_Scatter(a, local_n, MPI_DOUBLE, local_vec1, local_n,
            MPI_DOUBLE, 0, comm);

      b = malloc(local_n * comm_sz * sizeof(double));
      printf("Enter the second vector: \n");
      for (j = 0; j < local_n * comm_sz; j++) 
         scanf("%lf", &b[j]);

      MPI_Scatter(b, local_n, MPI_DOUBLE, local_vec2, local_n,
            MPI_DOUBLE, 0, comm);
   } else {
       MPI_Scatter(a, local_n, MPI_DOUBLE, local_vec1, local_n,
            MPI_DOUBLE, 0, comm);
       MPI_Scatter(b, local_n, MPI_DOUBLE, local_vec2, local_n,
            MPI_DOUBLE, 0, comm);
   }
}  /* Read_data */

/*-------------------------------------------------------------------*/
void Print_vector(double local_vec[], int local_n, int n, char title[], 
      int my_rank, MPI_Comm comm) {
   double* a = NULL;
   int i;
   
   if (my_rank == 0) {
      a = malloc(n * sizeof(double));
      MPI_Gather(local_vec, local_n, MPI_DOUBLE, a, local_n,
         MPI_DOUBLE, 0, comm);
      printf("%s: \n", title);
      for (i = 0; i < n; i++) {
         printf("%f", a[i]);
         if (i != (n - 1)) {
            printf(", ");
         }
      }
      printf("\n");
      free(a);
   } else {
       MPI_Gather(local_vec, local_n, MPI_DOUBLE, a, local_n,
         MPI_DOUBLE, 0, comm);  
   }

}  /* Print_vector */


/*-------------------------------------------------------------------*/
double Par_dot_product(double local_vec1[], double local_vec2[], 
      int local_n, MPI_Comm comm) {
   double dot_prod = 0;
   int i;
   for (i = 0; i < local_n; i++) {
      dot_prod += local_vec1[i] * local_vec2[i];
   }
   return dot_prod;
}

/*-------------------------------------------------------------------*/
void Par_vector_scalar_mult(double local_vec[], double scalar, 
      double local_result[], int local_n) {
   int i;
   for (i = 0; i < local_n; i++) {
      local_result[i] = local_vec[i] * scalar;
   }
}  /* Par_vector_scalar_mult */
