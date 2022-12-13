#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc,char *argv[]) {

   int numtasks, rank, dest, source, count, tag = 0;
   int inmsg, outmsg;
   double vet;
   int i;
   MPI_Status Stat;

   MPI_Init(&argc,&argv);
   MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   
   
   int tamanho = atoi(argv[1]);
   int trocas = atoi(argv[2]);
   
   vet = (double*) malloc(tamanho * sizeof(double));
   for (i = 0; i < tamanho; i++) 
      vet[i] = -1;
   
   if (rank == 0){
   
      double before = MPI_Wtime();
      
      MPI_Recv(vet, tamanho, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &Stat);

      while (tag++ < trocas){
         for (i = 0; i < tamanho; i++) vet[i] = 4;
         MPI_Send(vet, 1, sizeof(vet), 1, tag, MPI_COMM_WORLD);
      	 if (tag != trocas) 
      	 	MPI_Recv(vet, 1, sizeof(vet), 0, tag, MPI_COMM_WORLD, &Stat);
      }
      
      double final_time = MPI_Wtime() - before; 
      printf("Execution Time: %lf\n",final_time);
      
   } else {

         
     while (tag++ < trocas){
        for (i = 0; i < tamanho; i++) vet[i] = 2;
        MPI_Send(vet, 1, sizeof(vet), 1, tag, MPI_COMM_WORLD);
        if (tag != trocas) 
        	MPI_Recv(vet, 1, sizeof(vet), 0, tag, MPI_COMM_WORLD, &Stat);
     } 
     
   }
   
   MPI_Finalize();

} 
