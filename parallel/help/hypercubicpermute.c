/*	Cpt S 483, Introduction to Parallel Computing
 *	School of Electrical Engineering and Computer Science
 *	
 *	Example code
 *	Hypercubic permutation.
  // This code assumes that p is a power of 2


   // 1. implement the hypercubic permutation
   // : there will be lg p iterations
   // : during iteration i, rank j will communicate with rank j'
   // :			iff the i^th lsb(j) <is *not* equal to> i^th lsb(j')
   //
   // : for the individual communications let us use the MPI_Sendrecv primitive

 * */

#include <stdio.h>
#include <mpi.h> 
#include <assert.h>

int main(int argc,char *argv[])
{

   int rank,p;

   MPI_Init(&argc,&argv);
   MPI_Comm_rank(MPI_COMM_WORLD,&rank);
   MPI_Comm_size(MPI_COMM_WORLD,&p);

 //  printf("my rank=%d\n",rank);
 //  printf("Rank=%d: number of processes =%d\n",rank,p);
  //
   int temp = 1;
   int step;
   int iter=0;
   for(step=1;step<p;step=2*step) {
   		if(rank==0) {
			printf("Debug: Iteration #%d\n",iter);
			fflush(stdout);
		}
   	
   		int mate = rank^temp;  
		printf("Rank=%d: Mate =%d\n",rank,mate);
		fflush(stdout);


		int x=1,y=2;
		MPI_Status stat;
		MPI_Sendrecv(&x,1,MPI_INT,\
					 mate,0,\
					 &y,1,MPI_INT,\
					 mate,0,\
					 MPI_COMM_WORLD,&stat\
					);
		printf("Rank=%d: iter<%d>: rec'd message from %d\n",rank,iter,stat.MPI_SOURCE);
		fflush(stdout);


		temp = temp<<1;
		iter++;

		MPI_Barrier(MPI_COMM_WORLD);
   }
	

   MPI_Finalize();
}
