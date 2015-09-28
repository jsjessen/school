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


#define BIGPRIME 2147483647

// Get user input n and G
get_input()
{

}

// Initialize
GenerateInitialGoL()
{
    // Goal:
    //      - generate and store initial matrix in parallel
    //      - rank i owns rows [i*(n/p) ... (i+1)*(n/p)-1]
    
    // Implementation:
    //      - rank 0 generate p random numbers [1 to BIGPRIME] and distribute ith to rank i
    //
    //      -  each rank:
    //          - use given number as seed, to randomly generate n/p values [1 to BIGPRIME]
    //          - if kth random number is even, then kth cell fixxed in local portion of matrix, state=Alive, else state=Dead
}

// Run Game of Life
Simulate()
{
    // Run for G generations
    
    // For each gen in G:
    //      Barrier: synch cells to same generation
    //      Each rank i determines states for cells it owns
}

// Determines whether a cell should be alive or dead next generation
// Precondition: All cells have access to neighboring entries
DetermineState(int row, int col)
{
    return alive or dead
}

// Display
DisplayGoL()
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
