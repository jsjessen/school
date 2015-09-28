// stack.c
/* Implementation of a stack of linked Nodes.  The data portion of each
* Node is a double.  The stack is representd by the global static
* variable topNode which is a NodePtr that points to the top node in
* the stack.
*
* See stack.h for comments concerning the purpose and behavior of
* each function.
*
* This code accompanies Lab 06 of CptS 122, Spring 2013.
*
* John Schneider
*/

#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

static NodePtr topNode=NULL;

int stackIsEmpty(void) {
	if (topNode == NULL)
		return 1;
	else
		return 0;
} // stackIsEmpty()


Status stackPush(double x) {
	NodePtr tmp;

	tmp = (NodePtr)malloc(sizeof(Node));
	if (tmp == NULL) {
		printf("Memory allocation failed.  Cannot push value.\n");
		return FAILURE;
	}
	tmp->x = x;
	tmp->next = topNode;
	topNode = tmp;

	return SUCCESS;
} // stackPush()


Status stackDestroy(void) {
	while (!stackIsEmpty())
		if (stackPop() == FAILURE)
			return FAILURE;
	return SUCCESS;
} // stackDestroy()


Status stackPop(void) {
	NodePtr tmp;

	if (stackIsEmpty()) {
		printf("Cannot pop from an empty stack.\n");
		return FAILURE;
	}
	tmp = topNode;
	topNode = tmp->next;
	free(tmp);

	return SUCCESS;
} // stackPop()


Status stackTop(double *x) {
	if (stackIsEmpty()) {
		printf("Cannot get top of an empty stack.\n");
		return FAILURE;
	}

	*x = topNode->x;

	return SUCCESS;
} // stackTop()


void stackPrint() {
	int count=0;
	NodePtr tmp;

	tmp = topNode;

	while (tmp != NULL) {
		printf("%d: %g\n", count++, tmp->x);
		tmp = tmp->next;
	}

	return;
} // stackPrint()
