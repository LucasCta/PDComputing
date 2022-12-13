#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc,char *argv[]) {

   int numtasks, rank, dest, source, count, tag = 0;
   int inmsg, outmsg;
   MPI_Status Stat;

   MPI_Init(&argc,&argv);
   MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   if (rank == 0){
   
      clock_t before = clock();
      
      while (tag++ < argv[4]){
        //Recebe os vetores
        //Multiplica conteudo por 2
        //Envia os vetores
        MPI_Recv(vet1, 1, sizeof(vet1), 0, tag, MPI_COMM_WORLD, &Stat);
        MPI_Recv(vet2, 1, sizeof(vet2), 0, tag, MPI_COMM_WORLD, &Stat); 
        MPI_Recv(vet3, 1, sizeof(vet3), 0, tag, MPI_COMM_WORLD, &Stat);
        printf("%lf",vet1[1]);
      }
      
      double final_time = (double) (clock() - before) / CLOCKS_PER_SEC; 
      printf("Execution Time: %lf\n",final_time);
      
   } else {
    
     int i;
     vet1 = (double*) malloc(argv[1] * sizeof(double));
     for (i = 0; i < argv[1]; i++) 
        vet1[i] = 1;
     vet2 = (double*) malloc(argv[2] * sizeof(double));
     for (i = 0; i < argv[2]; i++) 
        vet2[i] = 1;
     vet3 = (double*) malloc(argv[3] * sizeof(double));
     for (i = 0; i < atoi(argv[3]); i++) 
        vet3[i] = 1;  
         
     while (tag++ < argv[4]){
        //Multiplica conteudo por 2
        for (i = 0; i < atoi(argv[1]); i++) vet1[i] *= 2;
        for (i = 0; i < atoi(argv[2]); i++) vet2[i] *= 2;
        for (i = 0; i < atoi(argv[3]); i++) vet3[i] *= 2; 
        //Envia os vetores
        MPI_Send(vet1, 1, sizeof(vet1), 1, tag, MPI_COMM_WORLD); 
        MPI_Send(vet2, 1, sizeof(vet2), 1, tag, MPI_COMM_WORLD); 
        MPI_Send(vet3, 1, sizeof(vet3), 1, tag, MPI_COMM_WORLD); 
        //free(vet1); free(vet2); free(vet3);
        //Recebe os vetores
        MPI_Recv(vet1, 1, sizeof(vet1), 0, tag, MPI_COMM_WORLD, &Stat);
        MPI_Recv(vet2, 1, sizeof(vet2), 0, tag, MPI_COMM_WORLD, &Stat); 
        MPI_Recv(vet3, 1, sizeof(vet3), 0, tag, MPI_COMM_WORLD, &Stat);
     } 
     
   }
   
   MPI_Finalize();

} 
