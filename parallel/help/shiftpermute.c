/*	Cpt S 483, Introduction to Parallel Computing
 *	School of Electrical Engineering and Computer Science
 *	
 *	Example code
 *	Send receive test:
 *    implement shift permutation (rank i sends to rank i-1 and receives from rank i+1)
 *    For timing use of C gettimeofday() is recommended.
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

   //printf("my rank=%d\n",rank);
  // printf("Rank=%d: number of processes =%d\n",rank,p);


   if(rank==0) {
   		int y=0;
		MPI_Status status;
		int src = rank+1;
   		MPI_Recv(&y,1,MPI_INT,src,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
		printf("Rank=%d: received message %d from rank %d\n",rank,y,status.MPI_SOURCE);
		fflush(stdout);
   } else 
   if (rank==(p-1)) {
		int x = rank;
		int dest = rank-1;
		MPI_Send(&x,1,MPI_INT,dest,0,MPI_COMM_WORLD);
		printf("Rank=%d: sent message %d to rank %d", rank,x,dest);
		fflush(stdout);
   }
   else {
   		// post send first and then recv.
		int x = rank;
		int dest = rank-1;
		MPI_Send(&x,1,MPI_INT,dest,0,MPI_COMM_WORLD);
		printf("Rank=%d: sent message %d to rank %d", rank,x,dest);
		fflush(stdout);

   		int y=0;
		MPI_Status status;
		int src = rank+1;
   		MPI_Recv(&y,1,MPI_INT,src,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
		printf("Rank=%d: received message %d from rank %d\n",rank,y,status.MPI_SOURCE);
		fflush(stdout);
   }

   MPI_Finalize();
}
