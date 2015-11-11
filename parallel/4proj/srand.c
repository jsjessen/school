// James Jessen, Joseph Villarreal
// Parallel Computing - Project 3
// Serial Random Number Generator

// Usage: serial_rand <output_size> <seed> <modulus> <multiplier> <increment>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

//#define DEBUG
#ifdef DEBUG
    #define DEFAULT_OUTPUT_SIZE 100
    #define DEFAULT_SEED 314159 
    #define DEFAULT_MODULUS 2147483647
    #define DEFAULT_MULTIPLIER 1103515245
    #define DEFAULT_INCREMENT 12345
#else
    #define DEFAULT_OUTPUT_SIZE 128
    #define DEFAULT_SEED 1234
    #define DEFAULT_MULTIPLIER 7 
    #define DEFAULT_INCREMENT 19 
    #define DEFAULT_MODULUS 9973 
#endif

int* serial_rand(const unsigned long n, const unsigned long s, 
           const unsigned long P, const unsigned long a, const unsigned long b);

int compare(const void * a, const void * b);

int main(int argc, char* argv[])
{
    // Check for user inputs, else use defaults
    const unsigned long n = (argc > 1) ? atoi(argv[1]) : DEFAULT_OUTPUT_SIZE;
    const unsigned long s = (argc > 2) ? atoi(argv[2]) : DEFAULT_SEED;
    const unsigned long P = (argc > 3) ? atoi(argv[3]) : DEFAULT_MODULUS;
    const unsigned long a = (argc > 4) ? atoi(argv[4]) : DEFAULT_MULTIPLIER;
    const unsigned long b = (argc > 5) ? atoi(argv[5]) : DEFAULT_INCREMENT;
    
    struct timeval t8, t9;
    double x_ms, y_ms, diff;

    //shift "n" to double the size of our input
    unsigned long temp = n;
    while(temp < 1000000000)
    {
        int* rands = serial_rand(temp, s, P, a, b);

        gettimeofday(&t8,NULL);
        qsort(rands, temp, sizeof(int), compare);
        gettimeofday(&t9,NULL);

        x_ms = (double)t8.tv_sec*1000000 + (double)t8.tv_usec;
        y_ms = (double)t9.tv_sec*1000000 + (double)t9.tv_usec;
        diff = (double)y_ms - (double)x_ms;

        //printf("n = %d,  time = %.0lf\n", n, diff);
        printf("%.0lf\n",diff);
        temp = temp << 1;
        free(rands);
    }

    return 0;
}

int* serial_rand(const unsigned long n, const unsigned long s, 
        const unsigned long P, const unsigned long a, const unsigned long b) 
{
    // Show the parameters being used 
    //printf("\n"
    //       "output_size = %d\n"
    //       "seed = %d\n" 
    //       "modulus = %d, multiplier = %d, increment = %d\n", n, s, P, a, b);  

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
                "\t serial_rand <output_size> <seed> <multiplier> <increment> <modulus>\n\n");
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
    struct timeval t1,t2;
    gettimeofday(&t1,NULL);
    for(int i = 0; i < n; i++)
    {
        rands[i] = (a*prev + b) % P;
        prev = rands[i];
    }
    gettimeofday(&t2,NULL);

    double x_ms , y_ms , diff;
    x_ms = (double)t1.tv_sec*1000000 + (double)t1.tv_usec;
    y_ms = (double)t2.tv_sec*1000000 + (double)t2.tv_usec;
    diff = (double)y_ms - (double)x_ms;

    //printf("%.0lf \r\n", diff);

    return rands;
}

int compare(const void * a, const void * b)
{
    return ( *(int*)a - *(int*)b );
}
