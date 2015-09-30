// James Jessen
// Parallel Computing
// Project 2 - Conway's Game of Life
//
// Game Rules:
//  
//      Based on state of preceding generation.
//
//      isLiving && neighbors < 2 => DIE
//      isLiving && neighbors > 3 => DIE
//      isLiving && neighbor 2 or 3 => LIVE
//      isDead && neighbors == 3 => LIVE
//
//      where neighbors = { N, S, E, W, NE, NW, SE, SW } 8
//
// Structure is a mesh, with boundary cases solved by wrapping.
//      first row/column, last row/column, corners for diagonal
//
// Assumptions:
//
//      - matrix is an n by n square
//      - n >> p is divisible by p, where p = #processors
//      - user will input number of generations, G
//
// Implementation:
//
//      - share work equally
//      - minimize communication between p
//      - minimize number of p each p needs to talk with
//
//     Block Decomposition     |    Column Decomposition
//                             |
//          1 1 2 2            |          1 2 3 4
//          1 1 2 2            |          1 2 3 4
//          3 3 4 4            |          1 2 3 4
//          3 3 4 4            |          1 2 3 4
//                             |
//        Borders: 2p          |        Borders: p 
//    Connections: 3-8 each    |    Connections: 2-3 each
// Implimentation: complicated | Implimentation: simple

#include <mpi.h> 
#include "helper.h"

#define CORNER_TL 218   //Top-left corner
#define CORNER_TR 191   //Top-right corner
#define CORNER_BL 192   //Bottom-left corner
#define CORNER_BR 217   //Bottom-right corner
#define BAR_HOR 196     //Horizontal bar
#define BAR_VERT 179    //Vertical bar

#define ROOT 0
#define BIGPRIME 2147483647 // (2^32 - 1) = largest prime storable by signed 32bit int
#define DEAD 0
#define ALIVE 1

// p: number of processors
// n: world is a n by n grid 
// G: number of generations to simulate

// Initialize
int** GenerateInitialGoL(int n)
{
    // Goal:
    //      - generate and store initial matrix in parallel
    //      - rank i owns rows [i*(n/p) ... (i+1)*(n/p)-1]

    int seed;

    // Generate p random numbers in interval [1 to BIGPRIME] and distribute ith to rank i
    if(rank = ROOT)
    {
        min = 1;
        max = BIGPRIME;
        range = max - min;

        srand(time(NULL));

        for(dest = 0; dest < p; dest++)
        {
            int rand_num = get_random_number(1, BIGPRIME);

            if(dest == ROOT)
                seed = rand_num; 
            else
                MPI_Send(&rand_num, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
        }
    }
    else
    {
        MPI_Recv(&seed, 1, MPI_INT, ROOT, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
        srand(seed);
    }

    num_rows = n / p; // owned by this rank
    int world[num_rows][n]; // local
    // world[generation(even or odd)][row][col]
    // world[0][2][4] = gen0 cell (2,4)
    // alternate generations
    
    // true_row = (rank * num_rows) + local_row

    // start_row = rank * num_rows;
    // end_row = ((rank + 1) * num_rows) - 1;
    
    // for each row owned by current rank
    for(row = 0; row < num_rows; row++)
    {
        for(i = 0; i < n; i++)
        {
            // Check if random number is odd
            if(1 & get_random_number(1, BIGPRIME))
                world[row][i] = DEAD;
            else
                world[row][i] = ALIVE;
        }
    }
}


// Run Game of Life
void Simulate(int n, int G, int** world) 
{
    int** g[2];
    int last = p - 1;

    g[0] = world;

    // Run for G generations
    for(gen = 0; gen < G; gen++)
    {
        // Barrier: synch cells to same generation
        MPI_Barrier();

        // Send data accross border
        if(rank == NODE)
            MPI_Send();

        // Send data accross border
        if(rank == last)
            MPI_Send();

        // Each rank i determines states for cells it owns
        DetermineState(world, row, col);

        if(n <= 16)
            DisplayGoL();
    }
}

// Determines whether a cell should be alive or dead next generation
// Precondition: All cells have access to neighboring entries
bool DetermineState(int** world, int row, int col)
{
    int num_alive = 0; // number of living neighbors

    for(i = -1; i <= 1; i++)
    {
        for(j = -1; j <= 1; j++)
        {
            if(i == 0 && j == 0)
                continue;

            if(row + i < 0 || row + i > n)
                MPI_Recv();

            if(world[row+i][col+j])
                num_alive++;
        }
    }

    if(num_alive == 3)
        return ALIVE;

    if(world[row][col] == ALIVE && num_alive == 2)
        return ALIVE;

    return DEAD;
}

// Display
void DisplayGoL()
{
    // not practical for large n, so display every x generations
    // where x is decided based on timings for specific input size
    //      n < 10 => x == 1
    //      n > 10000 => x == 1000 
    //      I just made this up but you get the idea

    // Gather entire matrix in rank 0
    //      send only alive cells for smaller msgs
    //      ok to not do this, for simplicity
    // Display it

    int i = 0, row = 0, col = 0;

    // Top border
    printf("\n %c", CORNER_TL);

    for (i = 0; i < n; i ++)
        printf("%c", BAR_HOR);

    printf("%c\n", CORNER_TR);

    // Interior
    for (row = 0; row < n; row ++)
    {
        printf(" %c", BAR_VERT);

        for (col = 0; col < n; col ++)
            printf("%c", Current(row, col)); //Cells

        printf("%c\n", BAR_VERT);
    }

    // Bottom border
    printf(" %c", CORNER_BL);

    for (i = 0; i < n; i ++)
        printf("%c", BAR_HOR);

    printf("%c\n", CORNER_BR);

    return;
}

int main(int argc, char *argv[])
{
    int n, G;
    int rank, p;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    printf("my rank = %d\n", rank);
    printf("Rank = %d: number of processes = %d\n", rank, p);

    if(argc == 3)
    {
        n = argv[1];
        G = argv[2];
    }
    else
        get_input(&n, &G);

    Simulate(n, G, GenerateInitialGoL(n));

    MPI_Finalize();
}

// Report:
//
//      - Totol runtime (exluding display) to run sim for G
//      - Average time per generation (excluding display)
//      - Average time per the display function
//      - A seperate time for each communication step
//          timer for MPI_Barrier
//          timer for MPI_Gather
//          timer for MPI_Bcast
//          etc...
//
//        From each rank, at the end before program terminates

// Experiments
//
// Use to test scalability of your code
//
// Measure the average time per generation (excluding display) as a function of #procs, for varying input sizes but fixed G.
// Create a table, rows for diff input sizes n, and col for #procs
// Grow n in powers of 2, starting at n=4 up to n=2^10 (or more based on runtime)
// Vary #procs in powers of 2 {1,2,4,8,16} in glx cluster
//
//      Plot and analyze using table data as specified by pdf instructions
//      Don't forget to interprete results
