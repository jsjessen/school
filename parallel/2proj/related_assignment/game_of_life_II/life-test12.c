// life-test12.c
/* Program to test the functions in life.c as well as run a Game of
* Life simulation.
* 
* CptS 122, Spring 2013
* HW 4
* John B. Schneider
*/

#include <stdio.h>
#include <time.h>
#include "life1.h"  // Doubly-subscripted array.
#include "life2.h"  // Linked structures.

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#define SLEEP_FACTOR 1000
#define SLEEP_FUNC Sleep
#else
#include <stdlib.h>
#include <unistd.h>
#define SLEEP_FACTOR 1000000
#define SLEEP_FUNC usleep
#endif


void print_help1(void);

void print_help2(void);

int main() {
	int nrow, ncol, ngen, count, choice, useLinkedStructures=0;
	float percent;
	int sleep; // Amount of "sleep" between generations.
	float tmp;


	if (useLinkedStructures) {
		printf("Using linked structures.\n");
		print_help2();
	} else {
		printf("Using doubly-subscripted arrays.\n");
		print_help1();
	}
	printf("\nEnter choice: ");

	while ((choice = getchar()) != 'q' &&
		choice != EOF) {
			switch (choice) {

			case 'A':
			case 'a':
				if (useLinkedStructures)
					printf("Number of alive cells: %d\n", lifeNumAlive2());
				else
					printf("Number of alive cells: %d\n", lifeNumAlive1());
				break;

			case 'I':
			case 'i':
				printf("Enter the number of rows and columns: ");
				scanf("%d %d", &nrow, &ncol);
				if (useLinkedStructures)
					lifeInit2(nrow, ncol);
				else
					lifeInit1(nrow, ncol);
				break;

			case 'N':
			case 'n':
				if (useLinkedStructures)
					lifePrintNeighbors2();
				else
					lifePrintNeighbors1();
				break;

			case 'P':
			case 'p':
				if (useLinkedStructures)
					lifePrint2();
				else
					lifePrint1();
				break;

			case 'R':
			case 'r':
				printf("Enter percent [0.0 to 100.0]: ");
				scanf("%f", &percent);
				if (useLinkedStructures)
					lifePopulateRandom2(percent);
				else
					lifePopulateRandom1(percent);
				break;

			case 'S':
			case 's':
				if (useLinkedStructures)
					lifeStep2();
				else
					lifeStep1();
				break;

			case 'G':
			case 'g':
				printf("Enter number of rows and columns: ");
				scanf("%d %d", &nrow, &ncol);

				printf("\nThere are a total of %d cells.\n"
					"Enter percent of cells initially alive: ", nrow * ncol);
				scanf("%f", &percent);

				printf("\nEnter desired number of generations [-1='infinite loop']: ");
				scanf("%d", &ngen);
				if (useLinkedStructures) {
					lifeInit2(nrow, ncol);
					lifePopulateRandom2(percent);
				} else {
					lifeInit1(nrow, ncol);
					lifePopulateRandom1(percent);
				}

				printf("\nYou can specify an amount of 'sleep',"
					"in seconds or fractions of a second,\n"
					"between generations.  Enter amount of sleep [0=none]: ");
				scanf("%f", &tmp);
				sleep = (int)(tmp * SLEEP_FACTOR);
				count = 0;
				while (1) {
					printf("Generation number: %d\n", count);
					if (useLinkedStructures) {
						lifeStep2();
						lifePrint2();
					} else {
						lifeStep1();
						lifePrint1();
					}

					if (sleep != 0)
						SLEEP_FUNC(sleep);

					if (useLinkedStructures) {
						if (lifeNumAlive2() == 0 || count == ngen)
							break;
					} else {
						if (lifeNumAlive1() == 0 || count == ngen)
							break;
					}


#ifdef _WIN32
					system("cls");
#else
					system("clear");
#endif

					count++;
				}

				break;

			case 'T':
			case 't':
				if (useLinkedStructures) {
					printf("Was using linked structures.\n"
						"Switching to doubly-subscripted array.\n");
					print_help1();
				} else {
					printf("Was using doubly-subscripted array.\n"
						"Switching to linked structures.\n");
					print_help2();
				}
				useLinkedStructures = (useLinkedStructures + 1) % 2;
				break;

			case 'H':
			case 'h':
				if (useLinkedStructures)
					print_help2();
				else
					print_help1();
				break;

			case '\n': /* ignore whitespace */
			case '\t':
			case ' ':
				printf("\nEnter choice: ");
				break;

			default:
				printf("Not a valid choice.\n");
				break; 
			} // end switch

	} // end while

	return 0;
} // main()

void print_help1(void) {
	printf("Choices:\n"
		"  a = lifeNumAlive1()        [number of alive cells]\n"
		"  i = lifeInit1()            [initialize boards]\n"
		"  p = lifePrint1()           [print the board]\n"
		"  n = lifePrintNeighbors1()  [print neighbor counts]\n"
		"  r = lifePopulateRandom1()  [populate the board randomly]\n"
		"  s = lifeStep1()            [step one generation]\n"
		"  t = toggle implementation\n"
		"  g = play Game of Life\n"
		"  h = [help -- this menu]\n"
		"  q = [quit]\n");
	return;
} // print_help1()

void print_help2(void) {
	printf("Choices:\n"
		"  a = lifeNumAlive2()        [number of alive cells]\n"
		"  i = lifeInit2()            [initialize boards]\n"
		"  p = lifePrint2()           [print the board]\n"
		"  n = lifePrintNeighbors2()  [print neighbor counts]\n"
		"  r = lifePopulateRandom2()  [populate the board randomly]\n"
		"  s = lifeStep2()            [step one generation]\n"
		"  t = toggle implementation\n"
		"  g = play Game of Life\n"
		"  h = [help -- this menu]\n"
		"  q = [quit]\n");
	return;
} // print_help2()