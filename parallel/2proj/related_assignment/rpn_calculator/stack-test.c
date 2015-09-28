// stack-test.c
/* Program to test the functions in stack.c as accompanies Lab 06 for
* CptS 122, Spring 2013.
* 
* John Schneider
*/

#include <stdio.h>
#include "stack.h"

void print_help(void);

int main() {
	int c;
	double x;

	print_help();
	printf("cmd: ");
	c = getchar();

	while (!feof(stdin)) {
		switch (c) {

		case 'I':
		case 'i':
			printf("stackIsEmpty() returns %d\n", stackIsEmpty());
			break;

		case 'O':
		case 'o':
			printf("status: %d\n", stackPop());
			break;

		case 'T':
		case 't':
			if (stackTop(&x) == SUCCESS) 
				printf("top value: %g\n", x);
			else
				printf("stackTop() failed.\n");
			break;

		case 'U':
		case 'u':
			printf("Enter value to push on stack: ");
			scanf(" %lf", &x);
			printf("status: %d\n", stackPush(x));
			break;

		case 'D':
		case 'd':
			printf("status: %d\n", stackDestroy());
			break;

		case 'P':
		case 'p':
			stackPrint();
			break;

		case '\n': // ignore whitespace
		case '\t':
		case ' ':
			printf("cmd: ");
			break;

		case 'H':
		case 'h':
			print_help();
			break;

		case 'Q':
		case 'q':
			printf("Thanks for coming!\n");
			return 0;
			break;

		default:
			printf("Unrecognized command.\n");
			break;

		} // end switch
		c = getchar();
	} // end while

	return 0;
}


void print_help(void) {
	printf("\nValid commands:\n"
		"  d = stackDestroy   [destroy/erase stack]\n"
		"  i = stackIsEmpty() [1 = empty; 0 = non-empty]\n"
		"  o = stackPop()     [remove top of stack]\n"
		"  p = stackPrint()   [print stack; top to bottom; with counter]\n"
		"  t = stackTop()     [obtain data at top of stack]\n"
		"  u = stackPush()    [push data onto stack]\n"
		"  h = help           [this menu]\n"
		"  q = quit\n\n"
		);
	return;
}