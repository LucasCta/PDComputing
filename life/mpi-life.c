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
    int    m, n, count=0;
    for (m = i-1; m < i+2; m++){
        for (n = j-1; n < j+2; n++) {
            if (n < 0 || n > sizeColumn) continue;
            count += board[m][n];
        }
    } count -= board[i][j];
    return count;
}

void mpi_life (bool ** board, bool ** nextBoard, int sizeRows, int sizeColumn) {
    int    i, j, a;
    for (i=1; i<sizeRows-1; i++) {
        for (j=0; j<sizeColumn; j++) {
            a = countAdjacency(board, sizeRows, sizeColumn, i, j);
            if (a == 2) nextBoard[i][j] = board[i][j];
            if (a == 3) nextBoard[i][j] = 1;
            if (a < 2) nextBoard[i][j] = 0;
            if (a > 3) nextBoard[i][j] = 0;
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
            if (i == numtasks-2) MPI_Send(&board[(i*numRows)][0], (size*(numRows+2+(size%(numtasks-1)))), MPI_C_BOOL, i+1, tag, MPI_COMM_WORLD); 
            else MPI_Send(&board[(i*numRows)][0], size*(numRows+2), MPI_C_BOOL, i+1, tag, MPI_COMM_WORLD); 
        }
        
        // Recieve Final Board
        for (i = 0; i < numtasks-1; i++) {
            if (i == numtasks-2) MPI_Recv(&board[(i*numRows)][0], (size*(numRows+2+(size%(numtasks-1)))), MPI_C_BOOL, i+1, tag, MPI_COMM_WORLD, &Stat); 
            else MPI_Recv(&board[(i*numRows)][0], size*(numRows+2), MPI_C_BOOL, i+1, tag, MPI_COMM_WORLD, &Stat); 
        }
        
        // Print Final Board
        for (i=0; i<size+2; i++) {
            for (j=0; j<size; j++) 
                printf ("%c", board[i][j] ? 'O' : '-');
            printf ("\n");
        } printf("\n\n");

        //Free Allocated Space for Board
        free(board[0]); 
        free(board);
        
    } else {
    
        //Recieves Size and Steps Information from Main
        MPI_Recv(&size, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &Stat);
        MPI_Recv(&steps, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &Stat);
        numRows = size / (numtasks-1);
        if (rank == numtasks-1) numRows += size%(numtasks-1);
        
        //Allocate Board Pieces
        bool ** board = alloc_2d_bool(numRows+2,size);
        bool ** nextBoard = alloc_2d_bool(numRows+2,size);
        for (i=0; i < size; i++){
            nextBoard[0][i] = 0;
            nextBoard[numRows+1][i] = 0;  
        } bool ** tmp;
        
        //Recieve Board
        MPI_Recv(&board[0][0], size*(numRows+2), MPI_C_BOOL, 0, tag, MPI_COMM_WORLD, &Stat);
        
        int s;
        for (s=0; s<steps; s++) {
        
            //Play Game of Life
            mpi_life(board,nextBoard,numRows+2,size);
            tmp = nextBoard; nextBoard = board; board = tmp;
            
            //Trades Edges
            if (numtasks > 2){
                if (rank == numtasks-1) {
                    MPI_Recv(&board[0][0], size, MPI_C_BOOL, rank-1, tag, MPI_COMM_WORLD, &Stat);
                    MPI_Send(&board[1][0], size, MPI_C_BOOL, rank-1, tag, MPI_COMM_WORLD); 
                } else if (rank == 1) {
                    MPI_Send(&board[numRows][0], size, MPI_C_BOOL, rank+1, tag, MPI_COMM_WORLD);  
                    MPI_Recv(&board[numRows+1][0], size, MPI_C_BOOL, rank+1, tag, MPI_COMM_WORLD, &Stat);
                } else {  
                    MPI_Recv(&board[0][0], size, MPI_C_BOOL, rank-1, tag, MPI_COMM_WORLD, &Stat);
                    MPI_Send(&board[numRows][0], size, MPI_C_BOOL, rank+1, tag, MPI_COMM_WORLD); 
                    MPI_Send(&board[1][0], size, MPI_C_BOOL, rank-1, tag, MPI_COMM_WORLD); 
                    MPI_Recv(&board[numRows+1][0], size, MPI_C_BOOL, rank+1, tag, MPI_COMM_WORLD, &Stat);
                }
            }
            
            #ifdef DEBUG
            // Print Board
            printf("\n\n=== Rank: %d Step: %d ===\n\n",rank,s);
            for (i=0; i<numRows+2; i++) {
                for (j=0; j<size; j++) 
                    printf ("%c", board[i][j] ? 'O' : '-');
                printf ("\n");
            } 
            printf("\n========================\n\n");
            #endif
            
        } 
        
        //Sends Final Board to Main
        MPI_Send(&board[1][0], size*numRows, MPI_C_BOOL, 0, tag, MPI_COMM_WORLD); 
        
        // Free Allocated Space for Boards
        free(board[0]); 
        free(nextBoard[0]);
        free(board); 
        free(nextBoard);
        
    }
  
    MPI_Finalize();
    
}
    
