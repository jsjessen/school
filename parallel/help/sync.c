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

void foo_critical(long long int);
void foo_atomic(long long int);
void foo_locks(long long int);

int main(int argc, char *argv[])
{
	long long int i, loops;

	// loop {number of iterations} [number of threads]

	if(argc<2) {
		printf("Usage: loop {number of iterations} [number of threads]\n");
		exit(1);
	}
	
	loops = atoll(argv[1]);
	printf("Info: number of iterations = %lld (%d)\n",loops, sizeof(long long int));

	int p=1;
	if(argc==3) {
		p = atoi(argv[2]);
		assert(p>=1);
		printf("Debug: number of requested threads = %d\n",p);
	}

	omp_set_num_threads(p);

	#pragma omp parallel
	{
		assert(p==omp_get_num_threads());
		//printf("Debug: number of threads set = %d\n",omp_get_num_threads());

		int rank = omp_get_thread_num();
		//printf("Info: Rank=%d: my world has %d threads\n",rank,p);
	}  // end of my omp parallel region


	// use critical section or atomic
	foo_critical(loops);
	foo_atomic(loops);

	// use locks
	foo_locks(loops);


	return 0;
}

/* Implements using omp critical 
 * */
void foo_critical(long long int n) {
	long long int a=0;
	long long int i;

	double time = omp_get_wtime();
	#pragma omp parallel for schedule(static) shared(a)	
	for(i = 0; i < n; i++)  
	{	
		#pragma omp critical
		{
			a = a+1;
		}
	}
	time = omp_get_wtime() - time;
	printf("Final value = %d \n ", a);
	printf("Critical: Total time = %f seconds \n ", time);
} // end foo_critical

/* Implements using omp atomic 
 * */
void foo_atomic(long long int n) {
	long long int a=0;
	long long int i;

	double time = omp_get_wtime();
	#pragma omp parallel for schedule(static) shared(a)	
	for(i = 0; i < n; i++)  
	{	
		#pragma omp atomic
			a+=1;
	}
	time = omp_get_wtime() - time;
	printf("Final value = %d \n ", a);
	printf("Atomic: Total time = %f seconds \n ", time);
}//end foo_atomic

/* Implements using omp lock functions
 * */
void foo_locks(long long int n) {
	long long int a=0;
	long long int i;

	omp_lock_t my_lock;

	// init lock
	omp_init_lock(&my_lock);

	double time = omp_get_wtime();
	#pragma omp parallel for schedule(static) shared(a)	
	for(i = 0; i < n; i++) 
	{	
		omp_set_lock(&my_lock);
		a+=1;
		omp_unset_lock(&my_lock);
	}
	omp_destroy_lock(&my_lock);
	
	time = omp_get_wtime() - time;
	printf("Final value = %d \n ", a);
	printf("Locks: Total time = %f seconds \n ", time);
} // end foo_locks

