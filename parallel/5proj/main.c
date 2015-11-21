// James Jessen
// 10918967
// Project 5 - Pi Estimator

// Usage: proj5.exe <number of throws> <number of threads>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define DEBUG 0
#define debug_print(fmt, ...) \
        do { if(DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

#define DEFAULT_NUM_THROWS 100000000
#define DEFAULT_NUM_THREADS 1

#define ACTUAL_PI 3.1415926535897932384626433832795028841971693993751058209749445923078

typedef enum { false, true } bool;

typedef struct Coords 
{
    double x;
    double y;
} Coord;

Coord throw_dart(struct drand48_data* randBuf);
bool hit_target(Coord* dart);
double distance(const Coord* c1, const Coord* c2);
void print_results(unsigned long numThreads, unsigned long numThrows, 
                   double pi, double accuracy, double runTime);

// This algorithm essentially throws a dart n times into a unit square (1x1) 
// and computes the fraction of times that dart falls into the embedded unit circle. 
// This fraction multiplied by 4 gives an estimate for Pi. 
int main (int argc, char* argv[])
{
    // Check for user inputs, else use defaults
    const unsigned long numThrows  = (argc > 1) ? atoi(argv[1]) : DEFAULT_NUM_THROWS;
    const unsigned long numThreads = (argc > 2) ? atoi(argv[2]) : DEFAULT_NUM_THREADS;

    struct drand48_data randBuf;
    double* rands;
    unsigned long seed, i, hitCount;
    Coord dart;
    double startTime, stopTime, runTime;

    startTime = omp_get_wtime(); // start timer

    hitCount = 0;
    omp_set_num_threads(numThreads);
    #pragma omp parallel private(i, dart, seed, randBuf)
    {
        seed = omp_get_thread_num() * 5003 + 2147483647;
        srand48_r(seed, &randBuf);
        debug_print("Thread %d) Seed = %lu\n", omp_get_thread_num(), seed);

        #pragma omp for reduction(+:hitCount)
        for(i = 0; i < numThrows; i++)
        {
            dart = throw_dart(&randBuf);

            if(hit_target(&dart))
                hitCount++;
        }
        debug_print("Thread %d) Hits = %lu\n", omp_get_thread_num(), hitCount);
    }
    double pi = (hitCount * 4) / (double)numThrows;

    stopTime = omp_get_wtime(); // stop timer
    runTime = (stopTime - startTime) * 1000; // milliseconds

    double accuracy = cabs((1 - (cabs(ACTUAL_PI - pi) / ACTUAL_PI)) * 100);
    print_results(numThreads, numThrows, pi, accuracy, runTime);

    return 0;
}

void print_results(unsigned long numThreads, unsigned long numThrows, 
        double pi, double accuracy, double runTime)
{
    printf("%lu,%lu,%.15f,%%%.15f,%f\n", numThreads, numThrows, pi, accuracy, runTime);

    debug_print("Threads = %u  Throws = %u\n", numThreads, numThrows);
    debug_print("Pi Estimate = %.15f\n", pi);
    debug_print("Pi Actual   = %.15f\n", ACTUAL_PI);
    debug_print("Accuracy    = %%%.13f\n", accuracy);
    debug_print("Runtime     = %f ms\n\n", runTime);
}

// Throw a dart randomly, such that it hits within a unit square
Coord throw_dart(struct drand48_data* randBuf)
{
    Coord dart;

    drand48_r(randBuf, &dart.x);
    drand48_r(randBuf, &dart.y);

    return dart;
}

bool hit_target(Coord* dart)
{
    static const double radius = 0.5; 
    static const Coord center = { .x = 0.5, .y = 0.5 };

    if(distance(dart, &center) <= radius)
    {
        debug_print("(%f, %f) HIT\n", dart->x, dart->y);
        return true;
    }
    else
    {
        debug_print("(%f, %f) MISS\n", dart->x, dart->y);
        return false;
    }
}

// Calculate the distance between two cartesian points
// using the Pythagorean theorem
double distance(const Coord* c1, const Coord* c2)
{
    return sqrt(pow(c2->x - c1->x, 2) + pow(c2->y - c1->y, 2));
}


/*
Pi Estimator

    The idea is not to get speedup, but to get better precision. Can compare output to value online.
    
    // Throw a dart at a unit square (1x1)
    // There is a circle inside with diameter 1, bounded by the square.
    
    Say you have a circle, bound (edges touch) by a unit square (1x1). So the radius is 1/2
    
    r = 0.5
    Area of circle = (pi)r^2 = pi / 4
    Area of square = (2r)^2 = 1
    
    x hits in circle
    n darts
    
    (x/n) = pi / 4
    4*(x/n) = pi
    
    For coding, have origin be bottom left corner of square.
    Generate 2 random numbers between 0 and 1, for x,y coords
    
    Use drand48_r() for generating random numbers.
    Refer to sumcomp.c example for how to get random numbers
*/
