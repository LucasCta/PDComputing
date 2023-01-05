CC=mpicc

ex1: MPI_Ex1.c
	$(CC) MPI_Ex1.c -o ex1.exe

ex2: MPI_Ex2.c
	$(CC) MPI_Ex2.c -o ex2.exe

ex3: MPI_Ex1.c
	$(CC) MPI_Ex3.c -o ex3.exe
