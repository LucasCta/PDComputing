#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc,char *argv[]) {

   int numtasks, rank, dest, source, count, tag=1;
   int inmsg, outmsg;
   MPI_Status Stat;

   MPI_Init(&argc,&argv);
   MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   if (rank == 0){
   
      FILE *fp; fp = fopen(argv[1], "w");
      clock_t before = clock();
      
      for (source = 0; source < (numtasks-1)*100; source++){
           MPI_Recv(&inmsg, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &Stat); 
           fprintf(fp, "Recieved %d from Task %d \n", inmsg, Stat.MPI_SOURCE);
      } 
      
      double final_time = (double) (clock() - before) / CLOCKS_PER_SEC; 
      printf("Execution Time: %lf\n",final_time);
      fclose(fp);
      
      
   } else {
   
   
   
   }
   
   MPI_Finalize();

} 
