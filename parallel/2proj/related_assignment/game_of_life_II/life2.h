// life2.h
/* Header file to accompany life2.c.
* 
* CptS 122, Spring 2013
* HW 4
* John B. Schneider
*/
#ifndef __LIFE2_H__
#define __LIFE2_H__

// Initialize the board(s) and associated variables.
void lifeInit2(int nrow, int ncol);

/* // Free the cells associated with the grids. */
/* void lifeDestroy2(void); */

// Randomly populate the current board with the specified
// percentage of alive cells.
void lifePopulateRandom2(float percent);

// Return the number of cell that are alive in the current board.
int lifeNumAlive2(void);

// Apply the Life rules to update the status of each cell.
void lifeStep2(void);

// Display the current life board; * = alive; space = dead.
void lifePrint2(void);

// Display the current board, but instead of showing whether a cell is
// alive or dead, using a single digit, show the number of neighbors
// each cell has. 
void lifePrintNeighbors2(void);

#endif // __LIFE2_H__