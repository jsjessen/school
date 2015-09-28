// rpn-calc.c
/* Code that functions as a RPN calculator.
* Reverse Polish Notation: 5 3 - returns 2
* Infix Notation: 5 - 3 = 2
*
* James S. Jessen
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "stack.h"

#define SIZE 100

void print_help();
Status get_xy(double *x, double *y);
Status get_x(double *x);


int main() {
	double x, y;
	char* token;
	char string[SIZE];

	print_help();
	printf("rpn: ");
	fgets(string, SIZE, stdin);    // Get initial "command".

	while (!feof(stdin)) {         // Keep reading lines while there is input.
		token = strtok(string, " "); // Get initial token.
		while (token != NULL) {   // Keep reading tokens while there is more to string.
			if ((token[0] >= '0' && token[0] <= '9') ||
				(token[0] == '.' && (token[1] >= '0' && token[1] <= '9')) ||
				((token[0] == '+' || token[0] == '-') && (token[1] >= '0' && token[1] <= '9')) ||
				((token[0] == '+' || token[0] == '-') && (token[1] == '.') && (token[2] >= '0' && token[2] <= '9')))
			{
				stackPush(atof(token));
			}
			else  // If not, it's an operator. 
			{
				switch (token[0]) {

				case '*':
					if (get_xy(&x, &y) == SUCCESS)
						stackPush(y * x);
					break;

				case '/':
					if (get_xy(&x, &y) == SUCCESS)
						stackPush(y / x);
					break;

				case '+':
					if (get_xy(&x, &y) == SUCCESS)
						stackPush(y + x);
					break;

				case '-':
					if (get_xy(&x, &y) == SUCCESS)
						stackPush(y - x);
					break;

				case '%':
					if (get_xy(&x, &y) == SUCCESS)
						stackPush( y - ((int)(y / x) * x) );
					break;

				case '^':
					if (get_xy(&x, &y) == SUCCESS)
						stackPush(pow(y, x));
					break;

				case 'n':
					if (get_x(&x) == SUCCESS)
						stackPush(-x);
					break;

				case 'q':
					if (get_x(&x) == SUCCESS)
						stackPush(pow(x, 2));
					break;

				case 'r':
					if (get_x(&x) == SUCCESS)
						stackPush(sqrt(x));
					break;

				case 's':
					if (get_x(&x) == SUCCESS)
						stackPush(sin(x));
					break;

				case 'c':
					if (get_x(&x) == SUCCESS)
						stackPush(cos(x));
					break;

				case 't':
					if (get_x(&x) == SUCCESS)
						stackPush(tan(x));
					break;

				case 'e':
					if (get_x(&x) == SUCCESS)
						stackPush(exp(x));
					break;

				case 'D':
					if (stackDestroy() == SUCCESS)
						printf("Stack destroyed/emptied.\n");
					break;

				case 'Q':
					printf("Bye!\n\n");
					exit(0);
					break;

				case 'H':
				case 'h':
					print_help();
					break;

				case '\n': // ignore whitespace
				case '\t':
				case ' ':
					printf("rpn: ");
					break;

				default:
					printf("Unrecognized command.\n");
					break;

				} // end switch
			} // end if operator 
			token = strtok(NULL, " "); // Get next token.
		}  // end while (token != Null)
		stackPrint();
		printf("rpn: ");
		fgets(string, SIZE, stdin);  // Get next "command".
	} // end while (!feof(stdin))

	return 0;
}


void print_help()
{
	printf("\nEnter numbers and/or operators. Valid operators are:\n"
		"  * = multiplication (y * x)   / = division (y / x)\n"
		"  + = addition (y + x)         - = subtraction (y - x)\n"
		"  %% = remainder (y %% x)        ^ = exponentiation (y ^ x)\n"
		"  n = negation (-x)\n"
		"  q = square (x * x)           r = sqrt(x)\n"
		"  s = sin(x)                   c = cos(x)\n"
		"  t = tan(x)                   e = exp(x)\n"
		"  D = clear (destroy) stack    Q = quit\n"
		"  h = help (this menu)\n"
		"Trig functions assume an argument in radians.\n\n"
		);

	return;
}

Status get_xy(double *x, double *y)
{
	if (stackTop(x) == FAILURE) {
		printf("Cannot get first operand.\n");
		return FAILURE;
	}
	stackPop();

	if (stackTop(y) == FAILURE) {
		stackPush(*x);
		printf("Cannot get second operand.\n");
		return FAILURE;
	}
	stackPop();

	return SUCCESS;
}

Status get_x(double *x)
{
	if (stackTop(x) == FAILURE) {
		printf("Cannot get first operand.\n");
		return FAILURE;
	}
	stackPop();

	return SUCCESS;
}