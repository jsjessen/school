#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const int p = 16;

//const int m = 8;
//const int n = 4;
//
//const int p = 16;

int GetMyMate(int r)
{
    // Avoid division by zero
    // rank 0 must be along the diagonal line of symmetry,
    // thus it doesn't need to exchange with another processor
    if(r == 0)
        return r;

    if(r < 0 || r > p - 1)
        return -1;

    // Get dimension of the square distributed mesh of processors
    int d = sqrt(p);

    int rRow = r / d; 
    int rCol = r % d; 

    // along the diagonal line of symmetry,
    // thus it doesn't need to exchange with another processor
    if(rRow == rCol)
        return r;

    //printf("r = %d [%d, %d]", r, rRow, rCol);

    // Find an r such that rRowT = rCol and colT = rRow
    for(int m = 1; m < p; m++)
    {
        int mRow = m / d;  
        int mCol = m % d;

        //printf("\n  m = %d [%d, %d]", m, mRow, mCol);

        if(mRow == rCol && mCol == rRow)
            return m;
    }
    return -1;
}

int main(int argc, char* argv[])
{
    // Validate: p 
    if(sqrt(p) - floor(sqrt(p)) > 1e-8)
    {
        printf("Error: p is not a perfect square\n");
        printf("sqrt(p) = %f\n", sqrt(p));
        exit(1);
    }

    for(int r = 0; r < p; r++)
    {
        int mate = GetMyMate(r);
        printf("\t%d <-> %d\n", r, mate);
    }

    int mate = GetMyMate(16);
    printf("\t%d <-> %d\n", -1, mate);

    return 0;
}
