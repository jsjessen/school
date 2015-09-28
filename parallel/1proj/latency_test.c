/*  James Jessen
 *  10918967
 *  Parallel Computing - Project #1
 *
 *  Modified example code:
 *  ----------------------------------------------------------
 *	    Cpt S 483, Introduction to Parallel Computing
 *	    School of Electrical Engineering and Computer Science
 *	    
 *	    Example code
 *	    Send receive test:
 *       	rank 1 sends to rank 0 (all other ranks sit idle)
 *       	For timing use of C gettimeofday() is recommended.
 *  ----------------------------------------------------------
 */

#include <stdio.h>
#include <mpi.h> 
#include <assert.h>
#include <sys/time.h>

#include <stdint.h>
typedef uint8_t byte_t;

#define MAX_SIZE 10000000
#define ITER_PER_SIZE 100

int mean(int arr[], int size)
{
    int sum = 0;
    for(int i = 0; i < size; i++)
        sum += arr[i];

    return sum / size;
}

int main(int argc, char *argv[])
{
    int rank, p;
    struct timeval t1, t2;
    byte_t buf[MAX_SIZE];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    printf("my rank = %d\n", rank);
    printf("Rank = %d: number of processes = %d\n", rank, p);

    assert(p >= 2);

    FILE *sendFile = fopen("send.dat", "a");
    FILE *recvFile = fopen("recv.dat", "a");
    if(sendFile == NULL || recvFile == NULL)
    {
        perror("Error: Failed to open file");
        MPI_Finalize();
        return 1;
    }

    // sync procs before collecting data
    //if(rank == 0)
    //{
    //    MPI_Send(&buf, 1, MPI_BYTE, 1, 0, MPI_COMM_WORLD); // send
    //    MPI_Barrier(MPI_COMM_WORLD); // sync send/recv
    //}
    //else
    //{
    //    MPI_Status status; // sender rank
    //    MPI_Recv(&buf, 1, MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status); // receive
    //    MPI_Barrier(MPI_COMM_WORLD); // sync send/recv
    //}

    for(int size = 1; size <= MAX_SIZE; size <<= 1)
    {
        int time[ITER_PER_SIZE];
        for(int i = 0; i < ITER_PER_SIZE; i++)
        {
            if(rank == 0)
            {
                int dest = rank + 1;
                gettimeofday(&t1, NULL); // t1
                MPI_Ssend(&buf, size, MPI_BYTE, dest, 0, MPI_COMM_WORLD); // send
                gettimeofday(&t2, NULL); // t2
                time[i] = (t2.tv_sec-t1.tv_sec)*1000 + (t2.tv_usec-t1.tv_usec)/1000; // t2 - t1
                MPI_Barrier(MPI_COMM_WORLD); // sync send/recv
            } 
            else
            {
                MPI_Status status; // sender rank
                gettimeofday(&t1, NULL); // t1
                MPI_Recv(&buf, size, MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status); // receive
                gettimeofday(&t2, NULL); // t2
                time[i] = (t2.tv_sec-t1.tv_sec)*1000 + (t2.tv_usec-t1.tv_usec)/1000; // t2 - t1
                MPI_Barrier(MPI_COMM_WORLD); // sync send/recv
            }
        }

        // record data
        if(rank == 0)
        {
            printf("%d -> %d: Send(%d) %d ms\n", rank, rank + p/2, size, mean(time, ITER_PER_SIZE)); 
            fprintf(sendFile, "%d,%d\n", size, mean(time, ITER_PER_SIZE)); 
        }
        else
        {
            printf("%d -> %d: Recv(%d) %d ms\n", rank, rank - p/2, size, mean(time, ITER_PER_SIZE));
            fprintf(recvFile, "%d,%d\n", size, mean(time, ITER_PER_SIZE));
        }
    }

    fclose(sendFile);
    fclose(recvFile);

    MPI_Finalize();
}
