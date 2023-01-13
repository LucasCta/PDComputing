#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool ** alloc_2d_bool (int rows, int cols) {
    bool *data = (bool *) malloc(rows*cols*sizeof(bool));
    bool **array = (bool **) malloc(rows*sizeof(bool*));
    for (int i=0; i<rows; i++)
        array[i] = &(data[cols*i]);
    return array;
}

int countAdjacency (bool ** board, int sizeRows, int sizeColumn, int i, int j) {
	int	m, n, count=0;
	for (m = i-1; m < i+2; m++){
		for (n = j-1; n < j+2; n++) {
	        if (n < 0 || n > sizeColumn) continue;
	        count += board[m][n];
	    }
	} count -= board[i][j];
	return count;
}

void mpi_life (bool ** board, bool ** nextBoard, int sizeRows, int sizeColumn) {
	int	i, j, a;
	for (i=1; i<sizeRows-1; i++) {
		for (j=0; j<sizeColumn; j++) {
		    a = countAdjacency(board, sizeRows, sizeColumn, i, j);
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
	int size, steps, numRows;
    	
    if (rank == 0) {
       
        //Read Board
	    FILE *f; f = stdin;
        fscanf(f,"%d %d",&size,&steps);
        
        // Allocate Board
	    bool ** board = alloc_2d_bool(size+2,size);
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
        } numRows = size / (numtasks-1);
        
        //Sending Board Pieces
        for (i = 0; i < numtasks-1; i++) {
            if (i == numtasks-2)  
                MPI_Send(&board[(i*numRows)][0], (size*(numRows+2+(size%(numtasks-1)))), MPI_C_BOOL, i+1, tag, MPI_COMM_WORLD); 
            else 
                MPI_Send(&board[(i*numRows)][0], size*(numRows+2), MPI_C_BOOL, i+1, tag, MPI_COMM_WORLD); 
        }
        
        /* Sends Processes 2 Tiles
        for (i=0; i<steps-1; i++) {
            MPI_Recv(&inmsg, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &Stat);
            for (j = 0; j < numtasks - 1; j++)
                MPI_Send(vet, tamanho, MPI_DOUBLE, 1, tag, MPI_COMM_WORLD);
        } MPI_Recv(&inmsg, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &Stat);
        */
        
        //Print Control
        for (i = 0; i < numtasks-1; i++) 
            MPI_Recv(&size, 1, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &Stat);
        
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
        numRows = size / (numtasks-1);
        if (rank == numtasks-1) numRows += size%(numtasks-1);
        
        //Allocate Board Pieces
        bool ** board = alloc_2d_bool(numRows+2,size);
        bool ** nextBoard = alloc_2d_bool(numRows,size);  
        
        //Recieve Board
        MPI_Recv(&board[0][0], size*(numRows+2), MPI_C_BOOL, 0, tag, MPI_COMM_WORLD, &Stat);
        
        // Print the Board
        printf("\n\n------Printing From Rank %d-------\n\n",rank);
        for (i=0; i<numRows+2; i++) {
	        for (j=0; j<size; j++) 
		        printf ("%c", board[i][j] ? 'O' : '-');
	        printf ("\n");
        } printf("\n----------------------------------\n\n");
       
        //1 step game of life
        mpi_life(board,nextBoard,numRows+2,size);
        
        // Print the Board
        printf("\n\n------NEWBOARD Printing From Rank %d-------\n\n",rank);
        for (i=0; i<numRows; i++) {
	        for (j=0; j<size; j++) 
		        printf ("%c", nextBoard[i][j] ? 'O' : '-');
	        printf ("\n");
        } printf("\n----------------------------------\n\n");
        
        //Print Control
        MPI_Send(&size, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
        
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
	
