// stack.h
/* Header file to accompany the stack.c file written for Task Lab 6 of
* CptS 122, Spring 2013.
*
* John Schneider
*/

#ifndef __STACK_H__
#define __STACK_H__

typedef struct node {
	double x;
	struct node* next;
} Node;

typedef Node* NodePtr;

typedef enum status {FAILURE, SUCCESS} Status;

// Return 1 if stack is empty, 0 otherwise.
int stackIsEmpty(void);

// Remove all the elements from the stack. 
// Return SUCCESS if successful, FAIL otherwise.
Status stackDestroy(void);

// Push given x value onto the stack (i.e., put it into a new
// Node at the top of the stack).
// Return SUCCESS if successful, FAIL otherwise.
Status stackPush(double x);

// Pop the top value off the stack (it is discarded).
// Return SUCCESS if successful, FAIL otherwise.
Status stackPop(void);

// Obtain the x value at the top of the stack.
// Return SUCCESS if successful, FAIL otherwise.
Status stackTop(double *x);

// Print the stack values by starting at the top node and walking down
// the stack.  Print a counter at the start of each line (starting
// from zero).
void stackPrint(void);

#endif // __STACK_H__