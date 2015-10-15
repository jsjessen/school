#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const int p = 16;

int GetMyMate(int r)
{
    int mate;
    int d = sqrt(p)

    int row = d / r;
    int col = d % r;

    // Find an r such that rowT = col and colT = row

    for(int i = 0; i < p; i++)
    {
        if(d / i == col && d % i == row)
            return i;
    }
    printf("FAILED\n");
    return -1;
}

void print_matrix(const int m, const int n, int matrix[m][n], int flag)
{
    int nrow = m;
    int ncol = n;

    putchar('\n');
    for(int i = 0; i <= m - 1; i++)
    {
        putchar('\t');
        for(int j = 0; j <= n - 1; j++)
        {
            printf(" %3d", matrix[i][j]);
        }
        putchar('\n');
    }
    putchar('\n');

    if(flag)
        putchar('\n');
}

void partition(const int m, const int n, const int p, int matrix[m][n], int flag)
{
    // PARTITION
    const int sm = (int)(m / sqrt(p));
    const int sn = (int)(n / sqrt(p));
    int sub[sm][sn];

    for(int r = 0; r <= p - 1; r++)
    {
        int km = (int)floor(r / sqrt(p));
        int kn = r % (int)sqrt(p);


        int count = 0;
        for(int i = 0; i <= sm - 1; i++)
        {
            for(int j = 0; j <= sn - 1; j++)
            {
                sub[i][j] = matrix[km*sm + i][kn*sn + j];
                count++;
            }
        }
        if(count != (n * m) / p)
            printf("Sub not filled correctly\n");
        printf("   r = %d", r);
        print_matrix(sm, sn, sub, flag);
    }
}

int main(int argc, char* argv[])
{
    const int m = 8;
    const int n = 4;

    putchar('\n');
    printf("m = %d\n", m);
    printf("n = %d\n", n);
    printf("p = %d\n", p);

    // VALIDATE P
    if(sqrt(p) - floor(sqrt(p)) > 1e-8)
    {
        printf("Error: p is not a perfect square\n");
        printf("sqrt(p) = %f\n", sqrt(p));
        exit(1);
    }

    // CREATE MATRIX
    int matrix[m][n];

    int count = 0;
    for(int i = 0; i <= m - 1; i++)
    {
        for(int j = 0; j <= n - 1; j++)
        {
            matrix[i][j] = count++;
        }
    }
    if(count != n * m)
        printf("Matrix not filled correctly\n");
    print_matrix(m, n, matrix, 0);
    //partition(m, n, p, matrix, 0);

    // TRANSFORM MATRIX
    int matrixT[n][m];

    for(int i = 0; i <= n - 1; i++)
    {
        for(int j = 0; j <= m - 1; j++)
            matrixT[i][j] = matrix[j][i];
    }
    printf("------------ TRANSFORM ------------\n");
    print_matrix(n, m, matrixT, 0);
    //partition(n, m, p, matrixT, 1);

    return 0;
}
