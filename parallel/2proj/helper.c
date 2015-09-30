#include "helper.h"

// Get user input n and G
get_input(int* n, int* G)
{
    printf("n = ");
    scanf("%d", n);

    printf("G = ");
    scanf("%d", G);
}

// Precondition: random number generator seeeded
int get_random_number(int min, int max)
{
    int range = max - min;
    return rand() % range + min;
}

// Returns true if odd, false if even
bool check_isOdd(int x)
{
    return x & 1;
}

clock_t start_timer()
{
    return clock();
}

int end_timer(clock_t start_time)
{
    end_time = clock();
    return (double)(end_time - start_time) / CLOCKS_PER_SEC; // in seconds
}
