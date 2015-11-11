// Parallel Random Number Generator

// NOTE TO SELF: Link math library (-lm)

// Usage: srand <output_size> <seed> <modulus> <multiplier> <increment>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "util.h"

#define DEBUG

#ifndef DEBUG
    #define OUTPUT_SIZE 100
    #define SEED time(NULL)
    #define MODULUS 2147483647
    #define MULTIPLIER 1103515245
    #define INCREMENT 12345
#else
    #define OUTPUT_SIZE 10
    #define SEED 13 
    #define MODULUS 50 
    #define MULTIPLIER 3 
    #define INCREMENT 2 
#endif

int* parallel_rand(const unsigned int n, const unsigned int s, 
                 const unsigned int P, const unsigned int a, const unsigned int b);

int main(int argc, char* argv[])
{
    // Check for user inputs, else use defaults
    const unsigned int n = (argc > 1) ? atoi(argv[1]) : OUTPUT_SIZE;
    const unsigned int s = (argc > 2) ? atoi(argv[2]) : SEED;
    const unsigned int P = (argc > 3) ? atoi(argv[3]) : MODULUS;
    const unsigned int a = (argc > 4) ? atoi(argv[4]) : MULTIPLIER;
    const unsigned int b = (argc > 5) ? atoi(argv[5]) : INCREMENT;

    parallel_rand(n, s, P, a, b);
    return 0;
}

void parallel_rand(const unsigned int n, const unsigned int s, 
                   const unsigned int P, const unsigned int a, const unsigned int b) 
{
    const unsigned int size = n/nump; // size of my block of the array
    int prefix[2] = [a, b];
    pp(rank, nump, size, P, prefix);

    int mate = rank + 1;
    if(rank == 0)
    {
        MPI_Send(prefix, 2, MPI_INT, mate, 0, MPI_COMM_WORLD);
        prefix = [a, b];
    }
    else
    {
        int tmp[2] = [prefix[0], prefix[1]];
        MPI_Sendrecv(tmp, 2, MPI_INT, mate, 0, \
                prefix, 2, MPI_INT, mate, 0, \
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    int rands[size];
    rands[0] = (prefix[0]*prev + prefix[1]) % P;
    int prev = rands[0]; 
    for(int i = 1; i < size; i++)
    {
        rands[i] = (a*prev + b) % P;
        prev = rands[i];
    }

    for(int i = 0; i < nump; i++)
    {
        if(rank == i)
            print_array_by_line(size, rands);

        MPI_Barrier(MPI_COMM_WORLD);
    }
}

// Parallel Prefix Operation
void pp(int rank, int nump, const unsigned int size, int P, int* prefix) 
{
    // p arrays containing: arrays with n/p elements

    for(int i = 0; i < size; i++)
    {
        prefix[1] = (prefix[1]*a + b) % P;
        prefix[0] = (prefix[0]*a) % P;
    }
    int total[2] = [prefix[0], prefix[1]];

    int depth = log2(nump);
    for(int i = 0; i < depth; i++)
    {
        // toggle the ith least significant bit of my rank to find mate
        int mate = rank ^ (1 << i); 

        // Exchange total with mate
        int mate_total;
        MPI_Sendrecv(total, 2, MPI_INT, mate, 0, \
                mate_total, 2, MPI_INT, mate, 0, \
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        total[1] = (total[1]*mate_total[0] + mate_total[1]) % P;
        total[0] = (total[0]*mate_total[0]) % P;

        // only update prefix with the totals from lower ranks 
        if(mate < rank)
        {
            prefix[1] = (prefix[1]*mate_total[0] + mate_total[1]) % P;
            prefix[0] = (prefix[0]*mate_total[0]) % P;
        }
    }

    return prefix;
}


