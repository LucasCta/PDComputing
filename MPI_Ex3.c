#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int power(int x, int y){
    if (y == 0) return 1;
    int temp = x;
    while (--y > 0){
        x *= temp;
    } return x;
}

int atoint(char * arg){
   int i, value, sizeArgv;
   value = 0; sizeArgv = 0;
   for (i = 0; arg[i] != '\0'; i++)
      sizeArgv += 1;
   for (i = 0; i < sizeArgv; i++)
      value +=  (arg[i] - '0') * power(10,sizeArgv-i-1);
   return value;
}

int main(int argc,char *argv[]) {

   int numtasks, rank, dest, source, count, tag = 0;
   int inmsg, outmsg, cont, i;
   int tamanho; int trocas;
   double * vet;
   MPI_Status Stat;

   MPI_Init(&argc,&argv);
   MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   tamanho = atoint(argv[1]); trocas = atoint(argv[2]);

   vet = (double*) malloc(tamanho * sizeof(double*));
   for (i = 0; i < tamanho; i++) 
      vet[i] = -1;
      
   if (rank == 0){
      
      double before = MPI_Wtime();
   
      while (cont++ < trocas){
         for (i = 0; i < tamanho; i++) 
            vet[i] = 4;
         MPI_Send(vet, tamanho, MPI_DOUBLE, 1, tag, MPI_COMM_WORLD);
      	 MPI_Recv(vet, tamanho, MPI_DOUBLE, 1, tag, MPI_COMM_WORLD, &Stat);
      } 
      
      double final_time = MPI_Wtime() - before; 
      printf("Execution Time: %lf\n",final_time);
      
   } else {
  
     while (cont++ < trocas){
        MPI_Recv(vet, tamanho, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &Stat);
        for (i = 0; i < tamanho; i++) 
            vet[i] = 2;
        MPI_Send(vet, tamanho, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);
     } 
     
   }
   
   MPI_Finalize();

} 
