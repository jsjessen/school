// life1.h
/* Header file to accompany life1.c.
* 
* CptS 122, Spring 2013
* HW 4
* John B. Schneider
*/
#ifndef __LIFE1_H__
#define __LIFE1_H__

// Initialize the board(s) and associated variables.
void lifeInit1(int nrow, int ncol);

// Randomly populate the current board with the specified
// percentage of alive cells.
void lifePopulateRandom1(float percent);

// Return the number of cell that are alive in the current board.
int lifeNumAlive1(void);

// Apply the Life rules to update the status of each cell.
void lifeStep1(void);

// Display the current life board; * = alive; space = dead.
void lifePrint1(void);

// Display the current board, but instead of showing whether a cell is
// alive or dead, using a single digit, show the number of neighbors
// each cell has. 
void lifePrintNeighbors1(void);

#endif // __LIFE1_H__