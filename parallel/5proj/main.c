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
#define debug_print(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

#define DEFAULT_N 10
#define DEFAULT_NUM_THREADS 1

#define M_PI 3.14159265358979323846

typedef enum { false, true } bool;

typedef struct Coords 
{
    double x;
    double y;
} Coord;

Coord center = { .x = 0.5, .y = 0.5 };

Coord throwDart();
bool hitTarget(Coord* dart);
double distance(const Coord* c1, const Coord* c2);

int main (int argc, char* argv[])
{
    // Check for user inputs, else use defaults
    const unsigned long n = (argc > 1) ? atoi(argv[1]) : DEFAULT_N;
    const int numThreads = (argc > 2) ? atoi(argv[2]) : DEFAULT_NUM_THREADS;


    double pi;
    Coord rand_coord;

    double diameter = 1.0;
    double radius = diameter / 2;

    struct drand48_data rand_buf;
    long int seed = 12345;
    double* rands;

    srand48_r(seed, &rand_buf);

    int hit_count = 0;
    for(int i = 0; i < n; i++)
    {
        drand48_r(&rand_buf, &rand_coord.x);
        drand48_r(&rand_buf, &rand_coord.y);

        if(hitTarget)
            hit_count++;
    }
    pi = (hit_count * 4) / n;

    printf("Pi     = %lf\n", pi);
    printf("GNU Pi = %lf\n", M_PI);
    double accuracy = abs(M_PI - pi) / M_PI * 100;
    printf("----------------------\n");
    printf("Accuracy = %%%lf\n", accuracy);

    return 0;
}

Coord throwDart()
{

}

bool hitTarget(Coord* dart)
{
    double radius = 0.5;
    if(distance(dart, &center) <= radius)
        return true;
    else
        return false;
}

double distance(const Coord* c1, const Coord* c2)
{
    return sqrt(pow(c2->x - c1->x, 2) + pow(c2->y - c1->y, 2));
}
