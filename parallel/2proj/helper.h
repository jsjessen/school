// James Jessen
// 10918967

#ifndef __HELPER_H__
#define __HELPER_H__


#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void enlist(PROC** list, PROC* new);
PROC* delist(PROC** list);
void printList(char* name, PROC* list);

#include "helper.h"

// Get user input n and G
get_input(int* n, int* G);

// Precondition: random number generator seeeded
int get_random_number(int min, int max);

// Returns true if odd, false if even
bool check_isOdd(int x);

clock_t start_timer();
int end_timer(clock_t start_time);


#endif
