#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc,char *argv[]) {

   int numtasks, rank, dest, source, count, tag=1;
   int inmsg, outmsg;
   MPI_Status Stat;

   MPI_Init(&argc,&argv);
   MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0){
       dest = rank+1; 
       outmsg = atoi(argv[1]);
       FILE *fp; fp = fopen(argv[2], "w");
       fprintf(fp, "The inicial value is %d \n", outmsg);
       MPI_Send(&outmsg, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
       for (source = 1; source < numtasks; source++){
            MPI_Recv(&inmsg, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &Stat); 
            fprintf(fp, "Recieved %d from Task %d \n", inmsg, source);
       } fclose(fp);
   } else if (rank == numtasks-1) {
       source = rank-1;
       MPI_Recv(&inmsg, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &Stat);
       outmsg = inmsg + rank; 
       MPI_Send(&outmsg, 1, MPI_INT, 0, tag, MPI_COMM_WORLD); 
   } else {
       source = rank-1;
       MPI_Recv(&inmsg, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &Stat); 
       dest = rank+1; 
       outmsg = inmsg + rank;
       MPI_Send(&outmsg, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
       MPI_Send(&outmsg, 1, MPI_INT, 0, tag, MPI_COMM_WORLD); 
   }

   MPI_Finalize();

} 
