#include "util.h"

void print_array(const unsigned int n, int arr[n])
{
    if(!arr)
        return;

    // Print input array
    putchar('\n');
    for(int i = 1; i <= n; i++)
    {
        printf(" %10d", arr[i-1]);
        if(i % 10 == 0)
            putchar('\n');
    }
    putchar('\n');
}

void print_array_by_line(const unsigned int n, int arr[n])
{
    if(!arr)
        return;

    // Print input array
    for(int i = 0; i < n; i++)
        printf("%d\n", arr[i]);
}

void print_matrix(const int nrow, const int ncol, int** matrix)
{
    putchar('\n');
    for(int m = 0; m <= nrow - 1; m++)
    {
        putchar('\t');
        for(int n = 0; n <= ncol - 1; n++)
        {
            printf(" %3d", matrix[m][n]);
        }
        putchar('\n');
    }
    putchar('\n');
}

// M1 * M2
int** mult_matrix(const unsigned int nrow1, const unsigned int ncol1, int M1[nrow1][ncol1], \
                  const unsigned int nrow2, const unsigned int ncol2, int M2[nrow2][ncol2])
{
    // Verify that the width of the first matrix equals the height of the second 
    if(ncol1 != nrow2)
    {
        fprintf(stderr, "Unable to multiply matrices of dimensions: %dx%d and %dx%d\n", 
                nrow1, ncol1, nrow2, ncol2);
        return NULL;
    }
    int shared = ncol1;

    // Create a matrix to store the product of the input matrices
    const unsigned int nrow = nrow1;
    const unsigned int ncol = ncol2;
    //int product[nrow][ncol];
    int **product = malloc(nrow*sizeof(*product) + (nrow*(ncol*sizeof(**product))));

    // Perform the calculations
    for(int row = 0; row < nrow; row++)
    {
        product[row] = (int*)(product + nrow + row*ncol); // initialize pointer for each row
        for(int col = 0; col < ncol; col++)
        {
            int sum = 0;
            for(int i = 0; i < shared; i++)
                sum += M1[row][i] * M2[i][col];

            product[row][col] = sum; // = Dot product of M1(row) and M2(col)
        }
    }
    return product;
}
 
// log base 2
double log2(double x)
{
    return log(x) / log(2);
}
