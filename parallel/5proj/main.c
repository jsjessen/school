// James Jessen
// 10918967
// Parallel Project 5 - Pi Estimator

// Usage: proj5.exe <n> <number of threads>
// Throw dart n times

#include <stdlib.h>
#include <math.h>

typedef enum bool { false, true };

typedef struct Coords 
{
    double x;
    double y;
} Coord;

Coord center = { .x = 0.5, .y = 0.5 };

Coord throwDart()
{

}

bool hitTarget(Coord* dart)
{
    if(distance(dart, &center) <= radius)
        return true;
    else
        return false;
}

double distance(const Coord* c1, const Coord* c2)
{
    return sqrt(pow(c2->x - c1->x, 2) + pow(c2->y - c1->y, 2));
}

int main(int argc, char* argv)
{
    double pi;
    Coord rand_coord;

    double diameter = 1.0;
    double radius = diameter / 2;

    struct drand48_data rand_buf;
    long int seedval = 12345;
    double* rands;

    srand48_r(seedval, rand_buf);
    drand48_r(rand_buf, rands);

    int hit_count = 0;
    for(int i = 0; i < n; i++)
    {
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
