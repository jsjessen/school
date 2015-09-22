/*	Cpt S 483, Introduction to Parallel Computing
 *	School of Electrical Engineering and Computer Science
 *	
 *	Example code

// Computing the sum of n numbers in parallel
// Assumptions: p is a power of 2
// 				n >> p and n divisible by p
// 				value of n passed as argument to the program 
// 				code will auto-generate n/p elements within each proc using a rand function.
//				
 * */

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h> 
#include <assert.h>
#include <time.h>

int rank,p,n;

void genInput(int *,int);

int main(int argc,char *argv[])
{

   MPI_Init(&argc,&argv);
   MPI_Comm_rank(MPI_COMM_WORLD,&rank);
   MPI_Comm_size(MPI_COMM_WORLD,&p);

 //  printf("my rank=%d\n",rank);
 //  printf("Rank=%d: number of processes =%d\n",rank,p);
  //
  if(argc!=2) {
  	printf("Usage: sum <n>\n");
	MPI_Finalize();
	return(1);
  }

  n = atoi(argv[1]);
  if(rank==0) {
  	printf("Info: input size n = %d\n",n);
  }

  assert(n%p==0);
  int size = n/p;

  //1. generate local array of size of n/p
  int *a=malloc(sizeof(int)*size);
  assert(a!=NULL);
  genInput(a,size);


  //2. compute local sum
  int i;
  int local_sum = 0;
  for(i=0;i<size;i++) {
  	local_sum+=a[i];
  }

  //3. compute sum in parallel
  //
  int global_sum=local_sum;

   int temp = 1;
   int step;
   int iter=0;
   for(step=1;step<p;step=2*step) {
   		if(rank==0) {
			printf("Info: Iteration #%d\n",iter);
			fflush(stdout);
		}
   	
   		int mate = rank^temp;  
	//	printf("Rank=%d: Mate =%d\n",rank,mate);
	//	fflush(stdout);

		int received_sum=0;

		MPI_Status stat;
		MPI_Sendrecv(&global_sum,1,MPI_INT,\
					 mate,0,\
					 &received_sum,1,MPI_INT,\
					 mate,0,\
					 MPI_COMM_WORLD,&stat\
					);
		global_sum+=received_sum;

	//	printf("Rank=%d: iter<%d>: rec'd message from %d\n",rank,iter,stat.MPI_SOURCE);
	//	fflush(stdout);


		temp = temp<<1;
		iter++;

		MPI_Barrier(MPI_COMM_WORLD);
   }
   
   printf("Rank=%d: Global sum = %d\n",rank,global_sum);
	

   MPI_Finalize();
}

void genInput(int *a,int size) {

	srand48(time(NULL)+rank);

	int i=0;
	for(i=0;i<size;i++) {
		a[i] = drand48()*100;
#ifdef __DEBUG__
		if(rank==0) printf("a[i]=%d\n",a[i]);
#endif
	}

#ifdef __DEBUG__
	char s[10000];
	strcpy(s,"");
	for(i=0;i<size;i++) {
		char temp[100];
		sprintf(temp,"%d ",a[i]);
		strcat(s,temp);
	}
	printf("Rank=%d: Input <%s>\n",rank,s);
#endif
}

