#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void print_matrix(int m, int n, int matrix[m][n])
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
}

int main(int argc, char* argv[])
{
    const int m = 4;
    const int n = 4;
    const int p = 4;

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
    print_matrix(m, n, matrix);

    // ----------------------------------------

    const int sm = (int)(m / sqrt(p));
    const int sn = (int)(n / sqrt(p));
    int sub[sm][sn];

    for(int r = 0; r <= p - 1; r++)
    {
        int km = (int)floor(r / sqrt(p));
        int kn = r % (int)sqrt(p);

        count = 0;
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
        print_matrix(sm, sn, sub);
    }

    return 0;
}
