#include "iostream"
#include "mpi.h"
using namespace std;

int main(int argc, char **argv)
{

	MPI_Init(&argc, &argv);
	MPI_Barrier(MPI_COMM_WORLD);
	double elapsed_time2 = -MPI_Wtime();

	int sum, id, p;
	int globalSolution = 0;

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	sum = id + 1;
	MPI_Reduce(&sum, &globalSolution, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	//	/* Stop timer */
	elapsed_time2 += MPI_Wtime();
	if (id == 0)
	{
		printf("The sum = %d\n", globalSolution);
		printf("Execution time %8.6f\n", elapsed_time2);
	}
	MPI_Finalize();

	system("pause");
	return 0;
}
/*
void sequencial(){
	
}
*/