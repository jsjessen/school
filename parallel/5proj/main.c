// James Jessen
// 10918967
// Parallel Project 5 - Pi Estimator

// Usage: proj5.exe <n> <number of threads>
// Throw dart n times

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define DEBUG
#ifdef DEBUG
    #define debug(...) printf(__VA_ARGS__)
#else
    #define debug(...)
#endif

#define DEFAULT_N 100000000
#define DEFAULT_NUM_THREADS 1

#define M_PI 3.14159

typedef enum { false, true } bool;

typedef struct Coords 
{
    double x;
    double y;
} Coord;

Coord throwDart();
bool hitTarget(Coord* dart);
double distance(const Coord* c1, const Coord* c2);

int main (int argc, char* argv[])
{

    // Check for user inputs, else use defaults
    const unsigned long n = (argc > 1) ? atoi(argv[1]) : DEFAULT_N;
    //const int numThreads = (argc > 2) ? atoi(argv[2]) : DEFAULT_NUM_THREADS;

    //Coord* rand_coord;


    //struct drand48_data rand_buf;
    long int seed = 12345;
    //double* rands;

    //srand48_r(seed, &rand_buf);
    srand(seed);

    int hit_count = 0;
    for(int i = 0; i < n; i++)
    {
        // drand48_r(&rand_buf, &rand_coord.x);
        // drand48_r(&rand_buf, &rand_coord.y);

        Coord dart = throwDart();
        if(hitTarget(&dart))
            hit_count++;
    }
    debug("hit count = %d\n", hit_count);

    double pi = (hit_count * 4) / (double)n;

    printf("Estimated Pi = %lf\n", pi);
    printf("Actual Pi    = %lf\n", M_PI);
    double accuracy = M_PI - pi / M_PI * 100;
    if(accuracy < 0)
        accuracy = -accuracy;
    printf("----------------------\n");
    printf("Accuracy = %%%lf\n", accuracy);

    return 0;
}

Coord throwDart()
{
    Coord dart;

    dart.x = rand() / (double)RAND_MAX; 
    dart.y = rand() / (double)RAND_MAX;

    if(dart.x > 1 || dart.x < 0 || dart.y > 1 || dart.y < 0)
        debug("(%lf, %lf)\n", dart.x, dart.y);

    return dart;
}

bool hitTarget(Coord* dart)
{
    //static const double diameter = 1.0;
    //static const double radius = diameter / 2;
    //static const Coord center = { .x = radius, .y = radius };
    static const double radius = 0.5; 
    static const Coord center = { .x = 0.5, .y = 0.5 };

    if(distance(dart, &center) <= radius)
        return true;
    else
        return false;
}

double distance(const Coord* c1, const Coord* c2)
{
    return sqrt(pow(c2->x - c1->x, 2) + pow(c2->y - c1->y, 2));
}


/*
   Project 5 - Due date: 11/17/2015 (Tuesday, 11:59pm PT) via OSBLE dropbox
   Assignment type: Individual 

   In this project you will implement a simple PI value estimator using the algorithm discussed in class. This algorithm essentially throws a dart n times into a unit square (1x1) and computes the fraction of times that dart falls into the embedded unit circle. This fraction multiplied by 4 gives an estimate for PI. 

   Your code should expect two arguments: <n> <number of threads>.
   Your code should output the PI value estimated at the end.  Note that the precision of the PI estimate could potentially vary with the number of threads (assuming n is fixed). 
   Your code should also output the total time (calculated using omp_get_wtime function). 

   -------------------------------------------------------------------------------

   Pi Estimator

   The idea is not to get speedup, but to get better precision. Can compare output to value online.

   Say you have a circle, bound (edges touch) by a unit square (1x1). So the radius is 1/2

   Area of circle = pi * r^2 = pi / 4
   Area of square = (2r)^2 = 1

   x hits in circle
   n darts

   (x/n) = pi / 4
   4*(x/n) = pi

   For coding, have origin be bottom left corner of square.
   Generate 2 random numbers between 0 and 1, for x,y coords

   Use drand48_r() for generating random numbers.
   Refer to sumcomp.c example for how to get random numbers

   -------------------------------------------------------------------------------

   Experiment for different values of n (starting from 1024 and going all the way up to a billion or more) and p (1,2,4..).  

   Please do two sets of experiments as discussed in class earlier, and report them as instructed below:

   1) For speedup - keeping n fixed and increase p (1,2,4). You may have to do this for large values of n to observe meaningful speedup. Note that the glx nodes have only 4 cores per node. So there is no need to increase the number of threads beyond 4. In your report show the run-time table for this testing and also the speedup chart. 

   2) For precision testing - keep n/p fixed, and increase p (1,2,.. up to 16 or 32). For this you will have to start with a good granularity (n/p) value which gave you some meaningful speedup from experiment set 1. The goal of this testing is to see if the PI value estimated by your code improves in precision with increase in n. Therefore, in your report make a table that shows the PI values estimated (up to some 20-odd decimal places) with each value of n tested. 




 * OpenMP implementation of Monte Carlo pi-finding algorithm
 * (based on an example in Chap. 17 of Quinn (2004))
 *
 * usage: pi <samples> <threads>

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main (int argc, char *argv[])
{
int i, count, samples, nthreads, seed;
struct drand48_data drand_buf;
double x, y;
double t0, t1;

samples  = atoi(argv[1]);
nthreads = atoi(argv[2]);
omp_set_num_threads (nthreads);

t0 = omp_get_wtime();
count = 0;

#pragma omp parallel private(i, x, y, seed, drand_buf) shared(samples)
{
seed = 1202107158 + omp_get_thread_num() * 1999;
srand48_r (seed, &drand_buf);

#pragma omp for reduction(+:count)
for (i=0; i<samples; i++) {
    drand48_r (&drand_buf, &x);
    drand48_r (&drand_buf, &y);
    if (x*x + y*y <= 1.0) count++;
}
}

t1 = omp_get_wtime();
printf("Estimate of pi: %7.5f\n", 4.0*count/samples);
printf("Time: %7.2f\n", t1-t0);
}
*/
