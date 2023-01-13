#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int countAdjacency (bool ** board, int size, int i, int j) {
	int	m, n, count=0;
	for (m = i-1; m < i+2; m++){
		for (n = j-1; n < j+2; n++){
	        if (m < 0 || n < 0) continue; 
	        if (m >= size || m >= size) continue;
	        count += board[m][n];
	    }
	} count -= board[i][j];
	return count;
}

void life (bool ** board, bool ** nextBoard, int size) {
	int	i, j, a;
	for (i=0; i<size; i++) {
		for (j=0; j<size; j++) {
		    a = countAdjacency(board, size, i, j);
			if (a == 2) nextBoard[i][j] = board[i][j];
			if (a == 3) nextBoard[i][j] = 1;
			if (a < 2) nextBoard[i][j] = 0;
			if (a > 3) nextBoard[i][j] = 0;
		}
    } 
}

int main () {
    
    int i,j;
	int size, steps;
	FILE *f; f = stdin;
	fscanf(f,"%d %d",&size,&steps);

    // Allocate Boards
	bool ** board = (bool **) malloc(sizeof(bool*)*size);
	for (i=0; i<size; i++) board[i] = (bool *) malloc(sizeof(bool)*size);
	bool ** nextBoard = (bool **) malloc(sizeof(bool*)*size);
	for (i=0; i<size; i++) nextBoard[i] = (bool *) malloc(sizeof(bool)*size);
	bool ** temp;
	
    // Get Board from File
	char *s = (char *) malloc(size+10);
	for (i=0; i<size; i++) {
		fgets (s, size*2, f);
		for (j=0; j<size; j++)
			board[i][j] = (s[j] == 'x');
	} free(s); fclose(f);
	
	// Play the Game of Life
	for (i=0; i<steps; i++){
		life(board,nextBoard,size);
		temp = nextBoard;
		nextBoard = board;
		board = temp;
    }
    
	// Print the Board
	for (i=0; i<size; i++) {
		for (j=0; j<size; j++) 
			printf ("%c", board[i][j] ? 'O' : '-');
		printf ("\n");
	}
	
	
    // Free Allocated Space for Boards
    for (i=0; i<size; i++)
        free(board[i]); 
    free(board);
    
}
	
