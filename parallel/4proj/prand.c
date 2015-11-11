// James Jessen, Joseph Villarreal
// Parallel Computing - Project 3
// Parallel Random Number Generator

#include "prand.h"

#ifdef DEBUG
    #define debug_print(...) printf(__VA_ARGS__)
#else
    #define debug_print(...)
#endif

// Caller must free result
void prand(const int rank, const int numProc, 
           const unsigned long outputSize, const unsigned long seed, 
           const unsigned long multiplier, const unsigned long increment,
           const unsigned long modulus, 
           const unsigned long size, int rands[size])
{
    assert(numProc % 2 == 0);

    if(rank == 0)
    {
        //printf("P = %d\nn = %d\n", numProc, outputSize);
    }

    if(!(0 < outputSize  && 
         0 < modulus  && 
         0 < multiplier  && multiplier < modulus && 
         0 <= increment && increment < modulus && 
         0 <= seed && seed < modulus))
        return;

    // Initialize prefix values
    unsigned long prefix[2] = {1, 0};
    if(rank == 0)
        prefix[0] = multiplier, prefix[1] = increment;

    // Run parallel prefix
    pp(rank, numProc, multiplier, increment, modulus, size, prefix);

    // Share prefix for use at the start of the next block
    if(rank == 0)
    {
        MPI_Send(prefix, 2, MPI_UNSIGNED_LONG, rank + 1, 0, MPI_COMM_WORLD);
        prefix[0] = multiplier; prefix[1] = increment;
    }
    else if(rank + 1 < numProc)
    {
        unsigned long sbuf[2] = {prefix[0], prefix[1]};
        MPI_Sendrecv(sbuf, 2, MPI_UNSIGNED_LONG, rank + 1, 0, \
                prefix, 2, MPI_UNSIGNED_LONG, rank - 1, 0, \
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    else
    {
        MPI_Recv(prefix, 2, MPI_UNSIGNED_LONG, rank - 1, 0, \
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // Use prefix to serially generate random numbers for your block
    rands[0] = (prefix[0]*seed + prefix[1]) % modulus;
    int prev = rands[0];
    for(int i = 1; i < size; i++)
    {
        rands[i] = (multiplier*prev + increment) % modulus;
        prev = rands[i];
    }
}

// Parallel Prefix Operation
void pp(const int rank, const int numProc, 
        const unsigned long multiplier, const unsigned long increment,
        const unsigned long modulus, const unsigned long size, unsigned long* prefix)
{
    // Serially calculate local prefix for your block
    unsigned long base[2] = {multiplier, increment};
    for(int i = 0; i < size; i++)
        combine(prefix, base, modulus);

    // Initialize total with prefix
    unsigned long total[2] = {prefix[0], prefix[1]};

    int depth = log2(numProc);
    for(int i = 0; i < depth; i++)
    {
        // Toggle the ith least significant bit of my rank to find mate
        int mate = rank ^ (1 << i); 

        // Exchange total with mate
        unsigned long mate_total[2];
        MPI_Sendrecv(total, 2, MPI_UNSIGNED_LONG, mate, 0, \
                mate_total, 2, MPI_UNSIGNED_LONG, mate, 0, \
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Combine the your total and your mate's total prefix values
        combine(total, mate_total, modulus);

        // Only update prefix with the totals from lower ranks 
        if(mate < rank)
            combine(prefix, mate_total, modulus);

        // Synchronize before moving to the next step
        MPI_Barrier(MPI_COMM_WORLD);
    }
}

// Combine the values and leave the result in x 
void combine(unsigned long* x, unsigned long* y, const unsigned long modulus)
{
    x[0] = (x[0]*y[0]) % modulus; 
    x[1] = (x[1]*y[0] + y[1]) % modulus;
}

double log2(double x)
{
    return log(x) / log(2);
}
