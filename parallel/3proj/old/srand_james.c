// Serial Random Number Generator

// Usage: srand <output_size> <seed> <modulus> <multiplier> <increment>

#include <stdio.h>
#include <stdlib.h>
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
    #define OUTPUT_SIZE 16
    #define SEED 17 
    #define MODULUS 50 
    #define MULTIPLIER 3 
    #define INCREMENT 2 
#endif

int* serial_rand(const unsigned int n, const unsigned int s, 
                 const unsigned int P, const unsigned int a, const unsigned int b);

int main(int argc, char* argv[])
{
    // Check for user inputs, else use defaults
    const unsigned int n = (argc > 1) ? atoi(argv[1]) : OUTPUT_SIZE;
    const unsigned int s = (argc > 2) ? atoi(argv[2]) : SEED;
    const unsigned int P = (argc > 3) ? atoi(argv[3]) : MODULUS;
    const unsigned int a = (argc > 4) ? atoi(argv[4]) : MULTIPLIER;
    const unsigned int b = (argc > 5) ? atoi(argv[5]) : INCREMENT;

    int* rands = serial_rand(n, s, P, a, b);
    print_array_by_line(n, rands);

    free(rands);
    return 0;
}

int* serial_rand(const unsigned int n, const unsigned int s, 
                 const unsigned int P, const unsigned int a, const unsigned int b) 
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
        return NULL;
    }

    int* rands = malloc(n * sizeof(int));
    if(rands < 0)
    {
        perror("Failed to allocate memory for random number array");
        return NULL;
    }
    
    // Generate random numbers using the Linear Congruential generating model
    int prev = s;
    for(int i = 0; i < n; i++)
    {
        rands[i] = (a*prev + b) % P;
        prev = rands[i];
    }
    return rands;
}
