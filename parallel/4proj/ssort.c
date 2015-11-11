// James Jessen, Joseph Villarreal
// Parallel Computing - Project 4
// Sample Sort 

// Usage: srand <output_size> <seed> <modulus> <multiplier> <increment>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#include <mpi.h> 

#include "prand.h"

#define DEFAULT_OUTPUT_SIZE 10
#define DEFAULT_SEED 1234
#define DEFAULT_MULTIPLIER 7 
#define DEFAULT_INCREMENT 19 
#define DEFAULT_MODULUS 9973 

#define ROOT 0

void ssort(int* locArray, int locArraySize);
void partition(int locArraySize, int locArray[locArraySize], 
               int numLocSplitters, int localSplitters[numLocSplitters]);
int compare(const void * a, const void * b);

int rank, numProc;

int main(int argc, char* argv[])
{
    // Initial MPI setup
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProc);

    // Check for user inputs, else use defaults
    const unsigned long n = (argc > 1) ? atoi(argv[1]) : DEFAULT_OUTPUT_SIZE;
    const unsigned long s = (argc > 2) ? atoi(argv[2]) : DEFAULT_SEED;
    const unsigned long a = (argc > 3) ? atoi(argv[3]) : DEFAULT_MULTIPLIER;
    const unsigned long b = (argc > 4) ? atoi(argv[4]) : DEFAULT_INCREMENT;
    const unsigned long P = (argc > 5) ? atoi(argv[5]) : DEFAULT_MODULUS;

    // Populate local array with random numbers 
    const unsigned long locArraySize = n / numProc;
    int locArray[locArraySize];
    printf("%d) 0\n", rank);
    prand(rank, numProc, n, s, a, b, P, locArraySize, locArray); 
    printf("%d) 1\n", rank);

    ssort(locArray, locArraySize);
    printf("%d) 2\n", rank);

    MPI_Finalize();
}

void ssort(int* locArray, int locArraySize)
{
    // S1) Every processor sorts its local array A_i, 
    //     and picks p − 1 evenly spaced pivots from the local sorted array. 
    //     These local pivots are also referred to as “local splitters”. 
    //--------------------------------------------------------------------------------

    // Sort local array
    qsort(locArray, locArraySize, sizeof(int), compare);

    int numLocSplitters = numProc - 1;
    int localSplitters[numLocSplitters];

    // Pick p - 1 evenly spaced pivots from the local sorted array 
    partition(locArraySize, locArray, numLocSplitters, localSplitters);

    // S2) The p(p − 1) splitters generated across all p processors are sorted, 
    //     and subsequently a subset of p − 1 evenly spaced pivots are selected. 
    //     These pivots are also referred to as the “global splitters”. 
    //     The sorting in this step can be either performed in parallel 
    //     (using another parallel sorting method such as bitonic sort) or in serial, 
    //     depending on the size of p. For the purpose of analysis in this chapter, 
    //     we will assume this step is performed using first an Allgather communication 
    //     of the local splitters, followed by each processor locally sorting 
    //     all p(p − 1) pivots to obtain the p − 1 global splitters.
    //--------------------------------------------------------------------------------
    // Use Allgather

    //An Allgather is now used to store all of the localSplitters
    //into the globalSplitters array on each processor
    int numGlobSplitters = numLocSplitters * numProc;
    int globalSplitters[numGlobSplitters];
    MPI_Allgather(localSplitters, numLocSplitters, MPI_INT, 
                  globalSplitters, numLocSplitters, MPI_INT, MPI_COMM_WORLD);

    // Sort the globalSplitters array
    qsort(globalSplitters, numGlobSplitters, sizeof(int), compare);

    int globalSubset[numProc - 1];
    partition(numGlobSplitters, globalSplitters, numProc-1, globalSubset);

    // S3) Using the global splitters, 
    //     each processor partitions its local array into p (possibly empty) parts, 
    //     marking the ith part to be sent to processor rank i.
    //--------------------------------------------------------------------------------

    int arraySize[numProc];
    int scounts[numProc];
    memset(scounts, 0, numProc);

    for(int i = 0; i < locArraySize; i++)
    {
        int p = 0;

        // First
        if(locArray[i] <= globalSubset[p])
        {
            scounts[p]++;
            continue;
        }

        // Middle
        for(p = 1; p < numProc; p++)
        {
            if(globalSubset[p-1] < locArray[i] && locArray[i] <= globalSubset[p])
            {
                scounts[p]++;
                continue;
            }
        }

        // Last
        scounts[p]++;
    }

    int sdisp[numProc];
    memset(sdisp, 0, numProc);
    for(int p = 1; p < numProc; p++)
        sdisp[p] += scounts[p-1];

    // S4) The arrays are redistributed using the Alltoallv transportation primitive.
    //--------------------------------------------------------------------------------
    // You may also use Alltoall to relay all the receive count values
    // Use Alltoallv

    int rcounts[numProc];

    /* tell the other processors how much data is coming */
    MPI_Alltoall(scounts, numProc, MPI_INT, 
                 rcounts, numProc, MPI_INT, 
                 MPI_COMM_WORLD);

    int sum = 0;
    for(int i = 0; i < numProc; i++)
        sum += rcounts[i];

    int rdisp[numProc];
    memset(rdisp, 0, numProc);
    for(int p = 1; p < numProc; p++)
        rdisp[p] += rcounts[p-1];

    // Allocate recv buffer
    int recvBuf[sum];

    /* send/rec different amounts of data to/from each processor */
    MPI_Alltoallv(locArray, scounts, sdisp, MPI_INT, 
            recvBuf,        rcounts, rdisp, MPI_INT, 
            MPI_COMM_WORLD);

    // S5) The set of elements received at every processor is then locally sorted. 
    //     This can either be achieved using a local sort of all the elements achieved, 
    //     or as a local merge operation of p sorted lists. 
    //     Note that the individual parts received from each source processor 
    //     in the Alltoallv step are already sorted internally.
    //     At the end of this step, 
    //     the concatentation of the local arrays (B_i) from rank 0 to rank p−1 
    //     in that order represents the final sorted output (B).
    //--------------------------------------------------------------------------------

    qsort(&recvBuf, sum, sizeof(int), compare);

    //Stop, you have finished sorting. Print all numbers by rank.
    for(int i = 0; i < numProc; i++)
    {
        if(rank == i)
            printf("%d ... %d\n", recvBuf[0], recvBuf[sum-1]);

        MPI_Barrier(MPI_COMM_WORLD);
    }
}

// Pick p - 1 evenly spaced pivots from the local sorted array 
void partition(int locArraySize, int locArray[locArraySize], 
        int numLocSplitters, int localSplitters[numLocSplitters])
{
    // Check if any work needs to be done
    if(numLocSplitters < 1)
        return;

    // Minimum width between splitters 
    int width = locArraySize / numProc; 

    // Pool of remainder that will be in last group,
    // unless distributed evenly such that no group has more than one extra
    int overflow = locArraySize % numProc;

    int i = 0;
    int pivot = 0;
    do
    {
        // While there are still remainders,
        // give each group an extra element to drain it
        pivot += width;
        if(overflow--)
            pivot++;

        localSplitters[i++] = locArray[pivot];
    } 
    while(i < numLocSplitters);
}

int compare(const void * a, const void * b)
{
       return ( *(int*)a - *(int*)b );
}
