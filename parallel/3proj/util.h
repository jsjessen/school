#ifndef __UTIL_H__
#define __UTIL_H__


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef enum { false, true } bool;

void print_array(const unsigned int n, int arr[n]);
void print_array_by_line(const unsigned int n, int arr[n]);
void print_matrix(const int nrow, const int ncol, int** matrix);

int** mult_matrix(const unsigned int nrow1, const unsigned int ncol1, int M1[nrow1][ncol1], \
                  const unsigned int nrow2, const unsigned int ncol2, int M2[nrow2][ncol2]);

double log2(double x);

#endif
