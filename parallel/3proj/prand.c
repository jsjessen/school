// James Jessen, Joseph Villarreal
// Parallel Computing - Project 3
// Parallel Random Number Generator

// Usage: srand <output_size> <seed> <modulus> <multiplier> <increment>

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <mpi.h> 
//#include "util.h"

#define DEFAULT_OUTPUT_SIZE 10000000
#define DEFAULT_SEED 17 
#define DEFAULT_MODULUS 50 
#define DEFAULT_MULTIPLIER 3 
#define DEFAULT_INCREMENT 2 

#define ROOT 0

void prand();
void pp(const unsigned long size, unsigned long* prefix) ;
void combine(unsigned long* x, unsigned long* y);
double log2(double x);

int rank, nump;

unsigned long n = DEFAULT_OUTPUT_SIZE;
unsigned long s = DEFAULT_SEED;
unsigned long P = DEFAULT_MODULUS;
unsigned long a = DEFAULT_MULTIPLIER;
unsigned long b = DEFAULT_INCREMENT;

int main(int argc, char* argv[])
{
    // Initial MPI setup
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nump);

    // Check for user inputs, else use defaults
    n = (argc > 1) ? atoi(argv[1]) : DEFAULT_OUTPUT_SIZE;
    s = (argc > 2) ? atoi(argv[2]) : DEFAULT_SEED;
    P = (argc > 3) ? atoi(argv[3]) : DEFAULT_MODULUS;
    a = (argc > 4) ? atoi(argv[4]) : DEFAULT_MULTIPLIER;
    b = (argc > 5) ? atoi(argv[5]) : DEFAULT_INCREMENT;

    if(rank == 0)
    {
        // Show the parameters being used 
        printf("\n"
               "output_size = %d\n"
               "seed = %d\n" 
               "modulus = %d, multiplier = %d, increment = %d\n", n, s, P, a, b);  

        // Verify input is valid
        if(!(0 < n  && 
             0 < P  && 
             0 < a  && a < P && 
             0 <= b && b < P && 
             0 <= s && s < P))
        {
            putchar('\n');
            fprintf(stderr, "Invalid input arguments for random number generation\n");
            printf("Usage:\n"
                   "\t srand <output_size> <seed> <multiplier> <increment> <modulus>\n\n");
            printf("Conditions:\n"
                   "\t 0 < n\n"
                   "\t 0 < P\n" 
                   "\t 0 < a < P\n"
                   "\t 0 <= b < P\n" 
                   "\t 0 <= s < P\n");
            putchar('\n');
            MPI_Finalize();
        }
    }

    // Generate n random numbers
    unsigned int temp = n;
    n = 32;
    struct timeval t1,t2;
    double x_ms , y_ms , diff;
    while(n < temp)
    {
        gettimeofday(&t1,NULL);
        prand();
        gettimeofday(&t2,NULL);

        x_ms = (double)t1.tv_sec*1000000 + (double)t1.tv_usec;
        y_ms = (double)t2.tv_sec*1000000 + (double)t2.tv_usec;
        diff = (double)y_ms - (double)x_ms;
        if(rank == 1)
        {
            printf("%.0lf \r\n", diff);
        }
        
        n = n << 1;
    }

    MPI_Finalize();
}

void prand()
{
    // Calculate the size of each partition
    const unsigned long size = n/nump; 

    // Initialize prefix values
    unsigned long prefix[2] = {1, 0};
    if(rank == 0)
        prefix[0] = a, prefix[1] = b;

    // Run parallel prefix
    pp(size, prefix);

    // Share prefix for use at the start of the next block
    if(rank == 0)
    {
        MPI_Send(prefix, 2, MPI_UNSIGNED_LONG, rank + 1, 0, MPI_COMM_WORLD);
        prefix[0] = a; prefix[1] = b;
    }
    else if(rank + 1 < nump)
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
    int rands[size];
    rands[0] = (prefix[0]*s + prefix[1]) % P;
    int prev = rands[0];
    int i;
    for(i = 1; i < size; i++)
    {
        rands[i] = (a*prev + b) % P;
        prev = rands[i];
    }

    //removed for production code
    // Display random numbers
    //int total_rands[n];
    //MPI_Gather(rands, size, MPI_INT, total_rands, size, MPI_INT, ROOT, MPI_COMM_WORLD);
    //if(rank == ROOT)
    //    print_array_by_line(n, total_rands);
}

// Parallel Prefix Operation
void pp(const unsigned long size, unsigned long* prefix) 
{
    // Serially calculate local prefix for your block
    unsigned long base[2] = {a, b};
    int i;
    for(i = 0; i < size; i++)
        combine(prefix, base);

    // Initialize total with prefix
    unsigned long total[2] = {prefix[0], prefix[1]};

    int depth = log2(nump);
    for(i = 0; i < depth; i++)
    {
        // Toggle the ith least significant bit of my rank to find mate
        int mate = rank ^ (1 << i); 

        // Exchange total with mate
        unsigned long mate_total[2];
        MPI_Sendrecv(total, 2, MPI_UNSIGNED_LONG, mate, 0, \
                mate_total, 2, MPI_UNSIGNED_LONG, mate, 0, \
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Combine the your total and your mate's total prefix values
        combine(total, mate_total);

        // Only update prefix with the totals from lower ranks 
        if(mate < rank)
            combine(prefix, mate_total);

        // Synchronize before moving to the next step
        MPI_Barrier(MPI_COMM_WORLD);
    }
}

// Combine the values and leave the result in x 
void combine(unsigned long* x, unsigned long* y)
{
    x[0] = (x[0]*y[0]) % P; 
    x[1] = (x[1]*y[0] + y[1]) % P;
}

double log2(double x)
{
    return log(x) / log(2);
}