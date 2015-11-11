// James Jessen, Joseph Villarreal
// Parallel Computing - Project 3
// Parallel Random Number Generator

#ifndef __PRAND_H__
#define __PRAND_H__


#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <mpi.h> 

void prand(const int rank, const int numProc, 
           const unsigned long outputSize, const unsigned long seed, 
           const unsigned long multiplier, const unsigned long increment,
           const unsigned long modulus, 
           const unsigned long size, int rands[size]);

void pp(const int rank, const int numProc, 
        const unsigned long multiplier, const unsigned long increment,
        const unsigned long modulus, const unsigned long size, unsigned long* prefix);

void combine(unsigned long* x, unsigned long* y, const unsigned long modulus);
double log2(double x);


#endif
