/* Example source code generated for Cpt S 483: Intro to Parallel Computing
 * Author: Ananth Kalyanaraman
 * Institution: Washington State University
 * Disclaimer: Use at your own risk!
 * */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <math.h>
#include <assert.h>

int p = 1;
int **generate_matrix(int ncols, int nrows);
void print_matrix(int **m, int nrows, int ncols);
void print_vector(int *v, int nrows);
int *generate_vector(int nrows);
int *multiply(int **m, int *v, int nrows, int ncols);
int *multiply_np(int **m, int *v, int nrows, int ncols);


int main(int argc, char *argv[])
{
	int nrows = 0, ncols = 0;
	int **matrix;
	int *vector; 
	int *end_vector;


	if(argc<3) 
	{
		printf("Usage: matrix_vector {matrix nrows} {matrix ncols} [number of threads] \n");
		exit(1);
	}
	
	nrows = atoi(argv[1]);		//ncols 
	ncols = atoi(argv[2]);		//nrows
	if(argc==4) 
	{
		p = atoi(argv[3]);
		assert(p>=1);
		printf("Debug: number of requested threads = %d\n",p);
	}

	omp_set_num_threads(p);
	
	double time = omp_get_wtime();
	matrix = generate_matrix(nrows, ncols);	
	double gen_mat_time = omp_get_wtime() - time;

	time = omp_get_wtime();
	vector = generate_vector(ncols);
	double gen_vec_time = omp_get_wtime() - time;


	//print_matrix(matrix, h, w);
	//print_vector(vector, h);

	time = omp_get_wtime();
	end_vector = multiply(matrix, vector, nrows, ncols);
	//end_vector = multiply_np(matrix, vector, nrows, ncols);
	double mxv_time = omp_get_wtime() - time;
	//print_vector(end_vector, h);
	double total_time = gen_mat_time + gen_vec_time + mxv_time;
	printf("Total time = %f sec (%f + %f + %f) \n ",\
		total_time,gen_mat_time,gen_vec_time,mxv_time);
	return 0;
}

// rowwise decomposition 
// linear scaling expected
int *multiply(int **m, int *v, int nrows, int ncols)
{
	int i = 0, j = 0;
	int *end_vec = (int *)malloc(sizeof(int)*nrows);
	assert(end_vec!=NULL);

	#pragma omp parallel for default(none) \
		private(i,j) shared(m,nrows,ncols,end_vec) firstprivate(v)
	for(i = 0; i < nrows; i++)
	{	
		//int rank = omp_get_thread_num();
		//printf("rank=%d: i=%d, p=%d\n",rank,i,p);

		end_vec[i] = 0;
		for(j = 0; j < ncols; j++)
		{
			end_vec[i] += m[i][j] * v[j];
		}
	}
	return end_vec;
}

//columnwise decomposition
// scaling expected to be impacted by the reduction operation 
int *multiply_np(int **m, int *v, int nrows, int ncols)
{
	int i = 0, j = 0;
	int *end_vec= NULL;
	int local_sum=0;
	end_vec = (int *)malloc(sizeof(int)*nrows);

	for(i=0;i<nrows;i++) {
		#pragma omp parallel for default(none) \
			private(j) shared(i,end_vec,m,ncols) firstprivate(v) reduction(+:local_sum)
		for(j=0;j<ncols;j++) {
			local_sum += m[i][j] * v[j];
		}
		end_vec[i]=local_sum;
	}

	return end_vec;
}

int **generate_matrix(int nrows, int ncols)
{
	int i = 0, j = 0, rank = 0, seed = 0;
	int **m = malloc(nrows * sizeof(int *));
	//#pragma omp parallel for shared(m)
	for(i = 0; i < nrows; i++)
	{
		m[i] = malloc(ncols * sizeof(int));
	}
	#pragma omp parallel for private(j)
	for (i = 0; i < nrows; i++)
	{
		for(j = 0; j < ncols; j++)
		{
			int rank = omp_get_thread_num();
			int seed = rank+1;
			seed = seed*i;		// big prime
			m[i][j] = rand_r(&seed)%10;	// small prime
		}
	}
	return m;
}

void print_matrix(int **m, int nrows, int ncols)
{
	int i = 0, j = 0;
	printf("--------------MATRIX-----------------\n\n");
	for (i = 0; i < nrows; i++)
	{
		for(j = 0; j < ncols; j++)
		{
			if(j == 0)
			{
				printf("|");
			}
			printf(" %d ", m[i][j]);
		}
		printf("|\n");
	}
	return;
}

void print_vector(int *v, int ncols)
{
	int i = 0;
	printf("--------------VECTOR-----------------\n\n");
	for(i = 0; i < ncols ; i++)
	{
		printf("| %d | \n", v[i]);
	}
	return;
}

int *generate_vector(int ncols)
{
	int i = 0, seed = 0, rank = 0;
	int *v = (int *)malloc(sizeof(int)*ncols);
	assert(v!=NULL);
	#pragma omp parallel for
	for(i = 0; i < ncols; i++)
	{
		int rank = omp_get_thread_num();
		int seed = rank +1;
		seed = seed*i;
		v[i] = rand_r(&seed)%10;
	}
	return v;
}
