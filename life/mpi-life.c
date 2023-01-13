#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int countAdjacency (bool ** board, int sizeColumn, int sizeLine, int i, int j) {
	int	m, n, count=0;
	for (m = i-1; m < i+2; m++){
		for (n = j-1; n < j+2; n++)
	        if (n < 0 || n > sizeLine) continue;
	        count += board[m][n];
	} count -= board[i][j];
	return count;
}

void mpi_life (bool ** board, bool ** nextBoard, int sizeColumn, int sizeLine) {
	int	i, j, a;
	for (i=1; i<sizeColumn-1; i++) {
		for (j=0; j<sizeLine; j++) {
		    a = countAdjacency(board, sizeColumn, sizeLine, i, j);
			if (a == 2) nextBoard[i-1][j] = board[i][j];
			if (a == 3) nextBoard[i-1][j] = 1;
			if (a < 2) nextBoard[i-1][j] = 0;
			if (a > 3) nextBoard[i-1][j] = 0;
		}
    }
}

int main (int argc,char *argv[]) {
    
    int numtasks, rank, dest, source, count, tag=1;
    int inmsg, outmsg;
    MPI_Status Stat;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   
    int i,j;   
	int size, steps, numColumns;
    	    
    if (rank == 0) {
       
        //Read Board
	    FILE *f; f = stdin;
        fscanf(f,"%d %d",&size,&steps);
        
        // Allocate Board
	    bool ** board = (bool **) malloc(sizeof(bool*)*(size+2));
	    for (i=0; i<size+2; i++) board[i] = (bool *) malloc(sizeof(bool)*size);
	    for (j=0; j<size; j++){
	        board[0][j] = false;
	        board[size+1][j] = false;
	    }
	    
        // Get Board from File
        char *s = (char *) malloc(size+10);
        for (i=1; i<size+1; i++) {
	        fgets (s, size+10, f);
	        for (j=0; j<size; j++)
		        board[i][j] = (s[j] == 'x');
        } free(s); fclose(f);
        
        // Sends Basic Information
        for (i=1; i < numtasks; i++) {
            MPI_Send(&size, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
            MPI_Send(&steps, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
        } numColumns = size / (numtasks-1);
         
        //Sending Board Pieces
	    MPI_Send(&board[0][0], size*(numColumns+2), MPI_C_BOOL, 1, tag, MPI_COMM_WORLD);
        //for (i = 1; i < numtasks - 2; i++)
        //    MPI_Send(&board[(i*numColumns)-1][0], size*(numColumns+2), MPI_C_BOOL, i+1, tag, MPI_COMM_WORLD);
        //MPI_Send(&board[((numtasks-1)*numColumns)-1][0], size*(numColumns+1), MPI_C_BOOL, numtasks - 1, tag, MPI_COMM_WORLD); 
        
        /*
        Sends Processes 2 Tiles
        for (i=0; i<steps-1; i++) {
            MPI_Recv(&inmsg, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &Stat);
            for (j = 0; j < numtasks - 1; j++)
                MPI_Send(vet, tamanho, MPI_DOUBLE, 1, tag, MPI_COMM_WORLD);
        } MPI_Recv(&inmsg, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &Stat);
        */
        
        // Print the Board
        for (i=0; i<size+2; i++) {
	        for (j=0; j<size; j++) 
		        printf ("%c", board[i][j] ? 'O' : '-');
	        printf ("\n");
        } printf("\n\n");

        //Free Allocated Space for Boards
        //for (i=0; i<size; i++)
        //    free(board[i]); 
        //free(board);
        
    } else {
    
        //Recieves Size and Steps Information from Main
        MPI_Recv(&size, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &Stat);
        MPI_Recv(&steps, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &Stat);
        numColumns = size / (numtasks-1);
        
        //Allocate Board Pieces Board (1+x+1 columns) newBoard(x columns)
	    bool ** board = (bool **) malloc(sizeof(bool*)*(numColumns+2));
	    for (i=0; i<numColumns+2; i++) board[i] = (bool *) malloc(sizeof(bool)*size);
	    bool ** nextBoard = (bool **) malloc(sizeof(bool*)*numColumns);
	    for (i=0; i<numColumns; i++) nextBoard[i] = (bool *) malloc(sizeof(bool)*size);
	    
	    //Recieve Board Piece (With 2 Extra Columns)
        MPI_Recv(&board[0][0], size*(numColumns+2), MPI_C_BOOL, 0, tag, MPI_COMM_WORLD, &Stat);
        
        // Print the Board
        for (i=0; i<numColumns+2; i++) {
	        for (j=0; j<size; j++) 
		        printf ("%c", board[i][j] ? 'O' : '-');
	        printf ("\n");
        } printf("\n\n");

        /*
	    //Play the Game of Life
	    for (i=0; i<steps; i++) {
	    
	        mpi_life(board,nextBoard,boardPiece,size);
	        
	        //Sends Back nextBoard
	        MPI_Send(, tamanho, MPI_DOUBLE, 1, tag, MPI_COMM_WORLD);
	        
	        //Recieve Att 2 Pieces
	        if (i != steps) {
	            MPI_Recv(vet, tamanho, MPI_DOUBLE, 1, tag, MPI_COMM_WORLD, &Stat);
	            board[1][0] = nextBoard;
            }
            
        }
        
        // Free Allocated Space for Boards
        
        for (i=0; i< numColumns+2; i++)
            free(board[i]); 
        for (i=0; i< numColumns; i++)
            free(nextBoard[i]);
        free(board); 
        free(nextBoard);*/
        
    }
  
    MPI_Finalize();
	
}
	
