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

#ifdef DEBUG
    #define debug_print(...) printf(__VA_ARGS__)
    #define debug_sync MPI_Barrier(MPI_COMM_WORLD);
#else
    #define debug_print(...)
    #define debug_sync 
#endif
#ifdef DEBUG2
    #define debug_print2(...) printf(__VA_ARGS__)
    #define debug_sync MPI_Barrier(MPI_COMM_WORLD);
#else
    #define debug_print2(...)
    #define debug_sync 
#endif

#define DEFAULT_OUTPUT_SIZE 22
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

double getTime(struct timeval t1, struct timeval t2);
double workTime;
struct timeval w1, w2;

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
    prand(rank, numProc, n, s, a, b, P, locArraySize, locArray); 
    debug_sync

        //for(int i = 0; i < locArraySize; i++)
        //debug_print("%d) locArray[%d] = %d\n", rank, i,  locArray[i]);

    struct timeval t1, t2;
    MPI_Barrier(MPI_COMM_WORLD);
    gettimeofday(&t1,NULL);
    ssort(locArray, locArraySize);
    MPI_Barrier(MPI_COMM_WORLD);
    gettimeofday(&t2,NULL);
    double totalTime = getTime(t1, t2);

    double totalWorkTime, totalCommTime;
    MPI_Reduce(&workTime, &totalWorkTime, 1, MPI_DOUBLE, MPI_MAX, ROOT, MPI_COMM_WORLD); 

    if(rank == ROOT)
    {
        totalWorkTime /= numProc;
        totalCommTime = totalTime - totalWorkTime;

        //printf("totalTime = %.0lf, workTime = %.0lf, commTime = %.0lf\n", 
        //        totalTime, totalWorkTime, totalCommTime);
        printf("%.0lf,%.0lf,%.0lf\n", totalTime, totalWorkTime, totalCommTime);
    }

    debug_print2("***FINALIZING***\n");
    MPI_Finalize();
}

void ssort(int* locArray, int locArraySize)
{
    // S1) Every processor sorts its local array A_i, 
    //     and picks p − 1 evenly spaced pivots from the local sorted array. 
    //     These local pivots are also referred to as “local splitters”. 
    //--------------------------------------------------------------------------------

    // Sort local array
    //1
    gettimeofday(&w1,NULL);
    qsort(locArray, locArraySize, sizeof(int), compare);
    gettimeofday(&w2,NULL);
    workTime += getTime(w1, w2);
    for(int p = 0; p < numProc; p++)
    {
        if(rank == p)
        {
            for(int i = 0; i < locArraySize; i++)
            {
                debug_print("%d) SORTED_1 locArray[%d] = %d\n", rank, i,  locArray[i]);
            }
            debug_print("------------------------------------------ \n");
        }
        debug_sync
    }
    debug_sync

        int numLocSplitters = numProc - 1;
    int localSplitters[numLocSplitters];

    // Pick p - 1 evenly spaced pivots from the local sorted array 
    partition(locArraySize, locArray, numLocSplitters, localSplitters);
    //for(int i = 0; i < numLocSplitters; i++)
    //debug_print("%d) PIVOTS localSplitters[%d] = %d\n", rank, i, localSplitters[i]);

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
    debug_sync
        MPI_Allgather(localSplitters, numLocSplitters, MPI_INT, 
                globalSplitters, numLocSplitters, MPI_INT, MPI_COMM_WORLD);
    for(int p = 0; p < 1; p++)
    {
        debug_sync
            if(rank == p)
            {
                for(int i = 0; i < numGlobSplitters; i++)
                    debug_print("%d) GLOBAL SPLITTERS globalSplitters[%d] = %d\n", rank, i, globalSplitters[i]);
            }
    }
    debug_sync

        // Sort the globalSplitters array
        //2
        gettimeofday(&w1,NULL);
    qsort(globalSplitters, numGlobSplitters, sizeof(int), compare);
    gettimeofday(&w2,NULL);
    workTime += getTime(w1, w2);
    for(int p = 0; p < 1; p++)
    {
        debug_sync
            if(rank == p)
            {
                for(int i = 0; i < numGlobSplitters; i++)
                    debug_print("%d) SORTED GLOBAL SPLITTERS globalSplitters[%d] = %d\n", rank, i, globalSplitters[i]);
            }
    }
    debug_sync

        int numSubSplitters = numProc - 1; 
    int globalSubset[numSubSplitters];
    partition(numGlobSplitters, globalSplitters, numSubSplitters, globalSubset);
    for(int p = 0; p < 1; p++)
    {
        debug_sync
            if(rank == p)
            {
                for(int i = 0; i < numProc - 1; i++)
                {
                    debug_print("%d) SUBSET globalSubset[%d] = %d\n", rank, i, globalSubset[i]);
                    debug_print2("%d) SUBSET globalSubset[%d] = %d\n", rank, i, globalSubset[i]);
                }
            }
    }
    debug_sync

        // S3) Using the global splitters, 
        //     each processor partitions its local array into p (possibly empty) parts, 
        //     marking the ith part to be sent to processor rank i.
        //--------------------------------------------------------------------------------

        for(int p = 0; p < numProc; p++)
        {
            if(rank == p)
            {
                for(int i = 0; i < locArraySize; i++)
                {
                    debug_print2("%d) SORTED_1 locArray[%d] = %d\n", rank, i,  locArray[i]);
                }
                debug_print2("------------------------------------------ \n");
            }
            debug_sync
        }

    debug_sync
        int scounts[numProc];
    for(int i = 0; i < numProc; i++)
        scounts[i] = 0;

    int loc_pos = 0;
    int p = 0;

    for(int splitter = 0; splitter < numSubSplitters; splitter++)
    {
        while(locArray[loc_pos] <= globalSubset[splitter] && loc_pos < locArraySize)
        {
            scounts[p]++;
            loc_pos++;
        }
        p++;
    }

    while(loc_pos < locArraySize)
    {
        scounts[p]++;
        loc_pos++;
    }

    debug_sync

        for(int p = 0; p < numProc; p++)
        {
            if(rank == p)
            {
                for(int i = 0; i < numProc; i++)
                {
                    debug_print2("%d) SEND COUNTS scounts[%d] = %d\n", rank, i, scounts[i]);
                }
                debug_print2("------------------------------------------ \n");
            }
            debug_sync
        }
    debug_sync

        int sdisp[numProc];
    for(int p = 0; p < numProc; p++)
        sdisp[p] = 0;

    int disp = 0;
    for(int p = 0; p < numProc; p++)
    {
        sdisp[p] = disp;
        disp += scounts[p];
    }
    debug_sync

        for(int p = 0; p < numProc; p++)
        {
            if(rank == p)
            {
                for(int i = 0; i < numProc; i++)
                {
                    debug_print2("%d) SEND DISPLACE sdisp[%d] = %d\n", rank, i, sdisp[i]);
                }
                debug_print2("------------------------------------------ \n");
            }
            debug_sync
        }
    debug_sync

        // S4) The arrays are redistributed using the Alltoallv transportation primitive.
        //--------------------------------------------------------------------------------
        // You may also use Alltoall to relay all the receive count values
        // Use Alltoallv

        int rcounts[numProc];
    for(int i = 0; i < numProc; i++)
        rcounts[i] = 0;

    debug_sync
        /* tell the other processors how much data is coming */
        debug_sync
        MPI_Alltoall(scounts, 1, MPI_INT, 
                rcounts, 1, MPI_INT, 
                MPI_COMM_WORLD);

    for(int p = 0; p < numProc; p++)
    {
        if(rank == p)
        {
            for(int i = 0; i < numProc; i++)
            {
                debug_print2("%d) RECV COUNTS rcounts[%d] = %d\n", rank, i, rcounts[i]);
            }
            debug_print2("------------------------------------------ \n");
        }
        debug_sync
    }
    debug_sync

        int sum = 0;
    for(int i = 0; i < numProc; i++)
        sum += rcounts[i];

    int rdisp[numProc];
    for(int p = 0; p < numProc; p++)
        rdisp[p] = 0;

    disp = 0;
    for(int p = 0; p < numProc; p++)
    {
        rdisp[p] = disp;
        disp += rcounts[p];
    }
    debug_sync
        for(int p = 0; p < numProc; p++)
        {
            if(rank == p)
            {
                for(int i = 0; i < numProc; i++)
                {
                    debug_print2("%d) RECV DISPLACE rdisp[%d] = %d\n", rank, i, rdisp[i]);
                }
                debug_print2("------------------------------------------ \n");
            }
            debug_sync
        }
    debug_sync

        // Allocate recv buffer
        int recvBuf[sum];

    /* send/rec different amounts of data to/from each processor */
    MPI_Alltoallv(locArray, scounts, 
            sdisp, MPI_INT, recvBuf,  
            rcounts, rdisp, MPI_INT, 
            MPI_COMM_WORLD);
    debug_sync

        // S5) The set of elements received at every processor is then locally sorted. 
        //     This can either be achieved using a local sort of all the elements achieved, 
        //     or as a local merge operation of p sorted lists. 
        //     Note that the individual parts received from each source processor 
        //     in the Alltoallv step are already sorted internally.
        //     At the end of this step, 
        //     the concatentation of the local arrays (B_i) from rank 0 to rank p−1 
        //     in that order represents the final sorted output (B).
        //--------------------------------------------------------------------------------

        //3
        gettimeofday(&w1,NULL);
    qsort(&recvBuf, sum, sizeof(int), compare);
    gettimeofday(&w2,NULL);
    workTime += getTime(w1, w2);
    debug_sync

        //Stop, you have finished sorting. Print all numbers by rank.
        //for(int i = 0; i < numProc; i++)
        //{
        //    if(rank == i)
        //        printf("%d) %d ... %d\n", rank, recvBuf[0], recvBuf[sum-1]);

        //    MPI_Barrier(MPI_COMM_WORLD);
        //}
        debug_sync
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
    int pivot = -1;

    do
    {
        // While there are still remainders,
        // give each group an extra element to drain it
        pivot += width;
        if(overflow-- > 0)
            pivot++;

        localSplitters[i++] = locArray[pivot];
    } 
    while(i < numLocSplitters);
}

int compare(const void * a, const void * b)
{
    return ( *(int*)a - *(int*)b );
}

double getTime(struct timeval t1, struct timeval t2)
{
    double x_ms, y_ms, diff;
    x_ms = (double)t1.tv_sec*1000000 + (double)t1.tv_usec;
    y_ms = (double)t2.tv_sec*1000000 + (double)t2.tv_usec;
    return (double)y_ms - (double)x_ms;
}
